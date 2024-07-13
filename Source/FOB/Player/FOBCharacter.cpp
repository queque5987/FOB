#include "Player/FOBCharacter.h"
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
#include "PCH.h"
#include "Net/UnrealNetwork.h"
#include "Player/CPlayerState.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "Weapon/Weapon.h"
#include "Player/CPlayerAnimBP.h"
//Deprecated
#include "CBullet.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


AFOBCharacter::AFOBCharacter()
{
	MinWalkSpeed = 300.f;
	MaxWalkSpeed = MinWalkSpeed;
	MaxSprintSpeed = 500.f;
	bReplicates = true;
	CameraBoomLength_TPS = 160.f;
	CameraBoomLength_FPS = -10.f;
	CameraRelativeLocationY_TPS = 45.f;
	CameraRelativeLocationY_FPS = 0.f;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 450.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CameraBoomLength_TPS; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 70.f));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeLocation(FVector(0.f, CameraRelativeLocationY_TPS, 0.f));

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));

	TextRenderComponent->SetupAttachment(GetMesh());
	TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(fHP)));
	TextRenderComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKFinder(TEXT("/Game/Resources/Character/PlayerAsset/Ch15_nonPBR"));
	if (SKFinder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SKFinder.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -87.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}


	GetMesh()->SetCollisionObjectType(COLLISION_CHANNEL_PLAYER);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	// Input Asset Load
	ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMapFinder(TEXT("/Game/Resources/Character/Input/IMC_Default"));
	ConstructorHelpers::FObjectFinder<UInputAction> JumpActionFinder(TEXT("/Game/Resources/Character/Input/IA_Jump"));
	ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(TEXT("/Game/Resources/Character/Input/IA_Move"));
	ConstructorHelpers::FObjectFinder<UInputAction> LookActionFinder(TEXT("/Game/Resources/Character/Input/IA_Look"));
	ConstructorHelpers::FObjectFinder<UInputAction> ShiftActionFinder(TEXT("/Game/Resources/Character/Input/IA_Shift"));
	ConstructorHelpers::FObjectFinder<UInputAction> LMBActionFinder(TEXT("/Game/Resources/Character/Input/IA_LMB"));
	ConstructorHelpers::FObjectFinder<UInputAction> RMBActionFinder(TEXT("/Game/Resources/Character/Input/IA_RMB"));

	if (InputMapFinder.Succeeded()) DefaultMappingContext = InputMapFinder.Object;
	if (JumpActionFinder.Succeeded()) JumpAction = JumpActionFinder.Object;
	if (MoveActionFinder.Succeeded()) MoveAction = MoveActionFinder.Object;
	if (LookActionFinder.Succeeded()) LookAction = LookActionFinder.Object;
	if (LMBActionFinder.Succeeded()) LMBAction = LMBActionFinder.Object;
	if (ShiftActionFinder.Succeeded()) ShiftAction = ShiftActionFinder.Object;
	if (LMBActionFinder.Succeeded()) LMBAction = LMBActionFinder.Object;
	if (RMBActionFinder.Succeeded()) RMBAction = RMBActionFinder.Object;

	// AnimInstance Load

	ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstFinder(TEXT("/Game/Resources/Character/PlayerAsset/Animation/Blueprint/BP_PlayerAnimBP"));
	if (AnimInstFinder.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimInstFinder.Class);
}

void AFOBCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AFOBCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateHP();

	//TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(fHP)));
	TextRenderComponent->SetText(FText::FromString(ViewRotation_Delta.ToString()));
	if (C_PlayerState == nullptr) return;

// Dash Move Speed Set
	if (C_PlayerState->GetPlayerAnimStatus(PLAYER_DASHING))
	{
		MaxWalkSpeed = FMath::Min(MaxWalkSpeed * 1.025f, MaxSprintSpeed);
	}
	else
	{
		MaxWalkSpeed = FMath::Max(MaxWalkSpeed * 0.95f, MinWalkSpeed);
	}

// Aiming

	if (HasAuthority())
	{
		ClientSendViewRotation_Delta();
	}

	SetAimingMode(C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING), DeltaSeconds);

// Aiming Adjust
	if (EquippedWeapon_R != nullptr)
	{
		AdjustNozzleToAimSpot(DeltaSeconds);
	}
}

void AFOBCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	C_PlayerState = Cast<ACPlayerState>(GetPlayerState());
	if (C_PlayerState == nullptr) return;

	C_PlayerState->SetupDelegates();
	C_AnimInstance = Cast<UCPlayerAnimBP>(GetMesh()->GetAnimInstance());
}

void AFOBCharacter::OnRep_MaxWalkSpeed()
{
	ServerSetMaxWalkSpeed(MaxWalkSpeed);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AFOBCharacter::OnRep_EquippedWeapon_R()
{
	EquippedWeapon_R->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightHandSocket"));
}

void AFOBCharacter::OnRep_EquippedWeapon_L()
{
	EquippedWeapon_L->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LeftHandSocket"));
}

void AFOBCharacter::AdjustNozzleToAimSpot(float DeltaSeconds)
{
	IWeapon* I_Weapon = Cast<IWeapon>(EquippedWeapon_R);
	if (C_PlayerState == nullptr|| I_Weapon == nullptr) return;

	//LineTrace Properties
	FHitResult HitResult;
	FVector CameraLocation = C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING) ? CameraBoom->GetComponentLocation() : FollowCamera->GetComponentLocation();
	FVector ViewVector = GetViewRotation().Vector();
	float MaxTargetDistance = 5000.f;
	//Weapon Properties
	FVector NozzleLocation = I_Weapon->GetFireSocketPos();
	FVector NozzleVector = EquippedWeapon_R->GetActorRotation().Vector();
	FCollisionQueryParams CollisionQueryParam = FCollisionQueryParams();
	CollisionQueryParam.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, CameraLocation + ViewVector * MaxTargetDistance, ECC_Pawn, CollisionQueryParam);
	FVector GoalZeroPoint = bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance;

	DrawDebugSphere(GetWorld(), bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance, 20.f, 32.f, bHit ? FColor::Green : FColor::Red);
	DrawDebugLine(GetWorld(), CameraLocation, bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance, bHit ? FColor::Green : FColor::Red);

	FVector ZeroingPoint(NozzleLocation + NozzleVector * MaxTargetDistance);
	FVector NozzleGoalVector = (GoalZeroPoint - NozzleLocation).GetSafeNormal();
	FRotator DeltaRot = NozzleGoalVector.Rotation() - NozzleVector.Rotation();

	DrawDebugSphere(GetWorld(), ZeroingPoint, 20.f, 32.f, FColor::Blue);
	DrawDebugLine(GetWorld(), NozzleLocation, ZeroingPoint, FColor::Blue);

	FRotator CurrWeight = ViewRotation_Delta_Zeroing;

	CurrWeight.Pitch += (1.f + FMath::Atan(FMath::Abs(DeltaRot.Pitch))) * DeltaRot.Pitch / 2.f * DeltaSeconds;
	CurrWeight.Yaw += (1.f + FMath::Atan(FMath::Abs(DeltaRot.Yaw))) * DeltaRot.Yaw / 2.f * DeltaSeconds;
	CurrWeight.Pitch = CurrWeight.Pitch > 0 ? FMath::Min(25.f, CurrWeight.Pitch) : FMath::Max(-25.f, CurrWeight.Pitch);
	CurrWeight.Yaw = CurrWeight.Yaw > 0 ? FMath::Min(25.f, CurrWeight.Yaw) : FMath::Max(-25.f, CurrWeight.Yaw);
	CurrWeight.Normalize();

	ServerSetViewRotation_Delta_Zeroing(CurrWeight);

	// V1
	////UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : AdjustNozzleToAimSpot %s"), *FollowCamera->GetComponentLocation().ToString());
	//IWeapon* I_Weapon = Cast<IWeapon>(EquippedWeapon_R);
	//if (C_PlayerState == nullptr || I_Weapon == nullptr) return;

	////LineTrace Properties
	//FHitResult HitResult;
	//FVector CameraLocation = C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING) ? CameraBoom->GetComponentLocation() : FollowCamera->GetComponentLocation();
	//FVector ViewVector = GetViewRotation().Vector();
	//float MaxTargetDistance = 5000.f;
	////Weapon Properties
	//FVector NozzleLocation = I_Weapon->GetFireSocketPos();
	//FVector NozzleVector = EquippedWeapon_R->GetActorRotation().Vector();
	//FCollisionQueryParams CollisionQueryParam = FCollisionQueryParams();
	//CollisionQueryParam.AddIgnoredActor(this);

	//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, CameraLocation + ViewVector * MaxTargetDistance, ECC_Pawn, CollisionQueryParam);
	//FVector GoalZeroPoint = bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance;

	//DrawDebugSphere(GetWorld(), bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance, 20.f, 32.f, bHit ? FColor::Green : FColor::Red);
	//DrawDebugLine(GetWorld(), CameraLocation, bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance, bHit ? FColor::Green : FColor::Red);

	//float X = FVector::Dist2D(NozzleLocation, HitResult.Location);
	//FVector NozzleToHitLocationVector = (HitResult.Location - NozzleLocation).GetSafeNormal();
	//float Theta = FMath::Acos(FVector::DotProduct(NozzleVector, NozzleToHitLocationVector));
	//float R = X / FMath::Cos(Theta);

	//FVector ZeroingPoint(NozzleLocation + NozzleVector * R);
	//FVector NozzleGoalVector = (GoalZeroPoint - NozzleLocation).GetSafeNormal();
	//FRotator DeltaRot = NozzleGoalVector.Rotation() - NozzleVector.Rotation();

	//DrawDebugSphere(GetWorld(), ZeroingPoint, 20.f, 32.f, FColor::Blue);
	//DrawDebugLine(GetWorld(), NozzleLocation, ZeroingPoint, FColor::Blue);

	//FRotator CurrWeight = ViewRotation_Delta_Zeroing;

	//CurrWeight.Pitch += (1 + FMath::Atan(FMath::Abs(DeltaRot.Pitch))) * DeltaRot.Pitch * DeltaSeconds;
	//CurrWeight.Yaw += (1 + FMath::Atan(FMath::Abs(DeltaRot.Yaw))) * DeltaRot.Yaw * DeltaSeconds;
	//CurrWeight.Pitch = CurrWeight.Pitch > 0 ? FMath::Min(25.f, CurrWeight.Pitch) : FMath::Max(-25.f, CurrWeight.Pitch);
	//CurrWeight.Yaw = CurrWeight.Yaw > 0 ? FMath::Min(25.f, CurrWeight.Yaw) : FMath::Max(-25.f, CurrWeight.Yaw);
	//CurrWeight.Normalize();

	//ServerSetViewRotation_Delta_Zeroing(CurrWeight);
}

void AFOBCharacter::ServerEquipItem_Implementation(AActor* WeaponToEquip)
{
	if (!UKismetSystemLibrary::DoesImplementInterface(WeaponToEquip, UWeapon::StaticClass())) return;

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : ServerEquipItem %s"), *WeaponToEquip->GetName());
	EquippedWeapon_R = WeaponToEquip;
	OnRep_EquippedWeapon_R();
}

void AFOBCharacter::ServerUnEquipItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : ServerUnEquipItem"));

	EquippedWeapon_R = nullptr;
}

void AFOBCharacter::UpdateHP_Implementation()
{
	if (C_PlayerState == nullptr) return;

	fHP = C_PlayerState->GetHP();
}

void AFOBCharacter::ServerSetMaxWalkSpeed_Implementation(float NewMaxWalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AFOBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFOBCharacter, fHP);
	DOREPLIFETIME(AFOBCharacter, MinWalkSpeed);
	DOREPLIFETIME(AFOBCharacter, MaxWalkSpeed);
	DOREPLIFETIME(AFOBCharacter, MaxSprintSpeed);
	DOREPLIFETIME(AFOBCharacter, EquippedWeapon_R);
	DOREPLIFETIME(AFOBCharacter, EquippedWeapon_L);
	DOREPLIFETIME(AFOBCharacter, ViewRotation_Delta);
	DOREPLIFETIME(AFOBCharacter, ViewRotation_Delta_Zeroing);
}


void AFOBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFOBCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFOBCharacter::Look);
		EnhancedInputComponent->BindAction(LMBAction, ETriggerEvent::Started, this, &AFOBCharacter::LMBTriggered);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Triggered, this, &AFOBCharacter::ShiftTriggered);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AFOBCharacter::ShiftCompleted);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Started, this, &AFOBCharacter::RMBStarted);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Completed, this, &AFOBCharacter::RMBCompleted);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFOBCharacter::LMBTriggered_Implementation()
{
	if (EquippedWeapon_R == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - EquippedWeapon_R Not Found"));
		return;
	}

	IWeapon* I_Weapon = Cast<IWeapon>(EquippedWeapon_R);
	if (I_Weapon == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - EquippedWeapon_R Not Valid Type"));
		return;
	}

	I_Weapon->LMBTriggered();

}

void AFOBCharacter::RMBStarted_Implementation()
{
	C_PlayerState->SetPlayerAnimStatus(PLAYER_AIMING, true);

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - RMBTriggered"));
}

void AFOBCharacter::RMBCompleted_Implementation()
{
	C_PlayerState->SetPlayerAnimStatus(PLAYER_AIMING, false);

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - RMBCompleted"));
}

void AFOBCharacter::ShiftTriggered_Implementation()
{
	C_PlayerState->SetPlayerAnimStatus(PLAYER_DASHING, true);
	UE_LOG(LogTemp, Log, TEXT("ShiftOnGoing : PLAYER_DASHING Set %s"), C_PlayerState->GetPlayerAnimStatus(PLAYER_DASHING) ? TEXT("True") : TEXT("False"));
}

void AFOBCharacter::ShiftCompleted_Implementation()
{
	C_PlayerState->SetPlayerAnimStatus(PLAYER_DASHING, false);
	UE_LOG(LogTemp, Log, TEXT("ShiftCompleted : PLAYER_DASHING Set %s"), C_PlayerState->GetPlayerAnimStatus(PLAYER_DASHING) ? TEXT("True") : TEXT("False"));
}

void AFOBCharacter::SetAimingMode_Implementation(bool e, float DeltaSeconds)
{
	FVector CamCurrLoc = FollowCamera->GetRelativeLocation();
	float ViewRotation_Delta_Pitch = ViewRotation_Delta.Pitch;
	if (ViewRotation_Delta_Pitch > 180.f) ViewRotation_Delta_Pitch -= 360.f;
	CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, e ? CameraBoomLength_FPS : CameraBoomLength_TPS, 0.5f);

	FollowCamera->SetRelativeLocation(
		FVector(
			e ? ( // Camera Move Forward/Backward At Aim Mode
				ViewRotation_Delta_Pitch > 0 ?
				ViewRotation_Delta_Pitch / 45.f * -15.f : ViewRotation_Delta_Pitch / 45.f * -30.f
				) : CamCurrLoc.X,
			FMath::Lerp(CamCurrLoc.Y, e ? CameraRelativeLocationY_FPS : CameraRelativeLocationY_TPS, 0.5f),
			e ? FMath::Abs(ViewRotation_Delta_Pitch / 45.f * 10.f) : CamCurrLoc.Z // Camera Move Up/DownWard At Aim Mode
		)
	);

	if (e)
	{
		if (CameraBoom->TargetArmLength - CameraBoomLength_FPS > 10.f) return;
		GetMesh()->HideBoneByName(FName("Head"), EPhysBodyOp::PBO_None);
	}
	else
	{
		GetMesh()->UnHideBoneByName(FName("Head"));
	}
}

void AFOBCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	//if (MoveVectorUpdated.IsBound()) MoveVectorUpdated.Execute(MovementVector);

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
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller == nullptr) return;

	float ViewRotation_Delta_Picth = ViewRotation_Delta.Pitch;
	if (ViewRotation_Delta_Picth > 180.f) ViewRotation_Delta_Picth -= 360.f;

	bool NoMoreUp = (LookAxisVector.Y < 0 && ViewRotation_Delta_Picth - LookAxisVector.Y > 45.f);
	bool NoMoreDown = (LookAxisVector.Y > 0 && ViewRotation_Delta_Picth - LookAxisVector.Y < -45.f);

	AddControllerYawInput(LookAxisVector.X);
	if (NoMoreUp || NoMoreDown)
	{
		GetController()->SetControlRotation(FRotator(NoMoreUp ? 45.f : -45.f, GetControlRotation().Yaw, GetControlRotation().Roll));
		return;
	}
	AddControllerPitchInput(LookAxisVector.Y);
}

void AFOBCharacter::ServerSetViewRotation_Delta_Zeroing_Implementation(FRotator NewViewRotation_Delta)
{
	ViewRotation_Delta_Zeroing = NewViewRotation_Delta;
}

void AFOBCharacter::OnRep_ViewRotation_Delta()
{
	//if (C_AnimInstance == nullptr) return;
	//C_AnimInstance->SetViewRotation_Delta(ViewRotation_Delta);
	//UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : OnRep_ViewRotation_Delta On %s\t%s"), HasAuthority() ? TEXT("Server") : TEXT("Client"), *ViewRotation_Delta.ToString());
}

void AFOBCharacter::ClientSendViewRotation_Delta_Implementation()
{
	ServerSetViewRotation_Delta(
		((GetViewRotation() - GetActorRotation()).GetNormalized() + ViewRotation_Delta_Zeroing)
	);
}

void AFOBCharacter::ServerSetViewRotation_Delta_Implementation(FRotator NewViewRotation_Delta)
{
	//FRotator ViewRotation_Delta_Weight = FRotator(10.f, 0.f, 0.f);
	//ViewRotation_Delta = (NewViewRotation_Delta + ViewRotation_Delta_Zeroing).GetNormalized();
	ViewRotation_Delta = NewViewRotation_Delta;
	OnRep_ViewRotation_Delta();
}


float AFOBCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damaged = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - Took Damage %f From %s"), DamageAmount, *DamageCauser->GetName());

	return Damaged;
}
