#include "FOBCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "FOBGameMode.h"
#include "Components/TextRenderComponent.h"
#include "CBullet.h"
#include "PCH.h"
#include "Net/UnrealNetwork.h"
#include "CPlayerState.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


AFOBCharacter::AFOBCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));

	TextRenderComponent->SetupAttachment(GetMesh());
	TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(fHP)));
	TextRenderComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

	GetMesh()->SetCollisionObjectType(COLLISION_CHANNEL_PLAYER);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AFOBCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AFOBCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateHP();
	
	TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(fHP)));
}

void AFOBCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ACPlayerState* C_PlayerState = Cast<ACPlayerState>(GetPlayerState());
	if (C_PlayerState == nullptr) return;

	C_PlayerState->SetupDelegates();
}

void AFOBCharacter::UpdateHP_Implementation ()
{
	ACPlayerState* C_PlayerState = Cast<ACPlayerState>(GetPlayerState());
	if (C_PlayerState == nullptr) return;

	fHP = C_PlayerState->GetHP();
}

void AFOBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFOBCharacter, fHP);
}


void AFOBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFOBCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFOBCharacter::Look);


		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Started, this, &AFOBCharacter::LMBTriggered);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFOBCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFOBCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFOBCharacter::LMBTriggered_Implementation()
{
	AFOBGameMode* GM = Cast<AFOBGameMode>(GetWorld()->GetAuthGameMode());
	if (GM == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Bang"));
		return;
	}
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	GetWorld()->SpawnActor<ACBullet>(ACBullet::StaticClass(), GetActorLocation(), GetBaseAimRotation(), SpawnParam);

	//GM->SpawnPotato(GetActorLocation(), GetBaseAimRotation() + FRotator(0.f, 0.f, 0.f));
}

float AFOBCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damaged = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - Took Damage %f From %s"), DamageAmount, *DamageCauser->GetName());

	return Damaged;
}
