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
#include "Player/CPlayerWidgetComponent.h"
#include "Weapon/CGrenadeAim.h"
#include "Components/SplineMeshComponent.h"
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
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
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
	ConstructorHelpers::FObjectFinder<UInputAction> CrouchActionFinder(TEXT("/Game/Resources/Character/Input/IA_Crouch"));
	ConstructorHelpers::FObjectFinder<UInputAction> ScrollActionFinder(TEXT("/Game/Resources/Character/Input/IA_Scroll"));
	ConstructorHelpers::FObjectFinder<UInputAction> GrenadeActionFinder(TEXT("/Game/Resources/Character/Input/IA_Q"));

	if (InputMapFinder.Succeeded()) DefaultMappingContext = InputMapFinder.Object;
	if (JumpActionFinder.Succeeded()) JumpAction = JumpActionFinder.Object;
	if (MoveActionFinder.Succeeded()) MoveAction = MoveActionFinder.Object;
	if (LookActionFinder.Succeeded()) LookAction = LookActionFinder.Object;
	if (LMBActionFinder.Succeeded()) LMBAction = LMBActionFinder.Object;
	if (ShiftActionFinder.Succeeded()) ShiftAction = ShiftActionFinder.Object;
	if (LMBActionFinder.Succeeded()) LMBAction = LMBActionFinder.Object;
	if (RMBActionFinder.Succeeded()) RMBAction = RMBActionFinder.Object;
	if (CrouchActionFinder.Succeeded()) CrouchAction = CrouchActionFinder.Object;
	if (ScrollActionFinder.Succeeded()) ScrollAction = ScrollActionFinder.Object;
	if (GrenadeActionFinder.Succeeded()) GranadeAction = GrenadeActionFinder.Object;

	// AnimInstance Load

	ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstFinder(TEXT("/Game/Resources/Character/PlayerAsset/Animation/Blueprint/BP_PlayerAnimBP"));
	if (AnimInstFinder.Succeeded()) GetMesh()->SetAnimInstanceClass(AnimInstFinder.Class);

	FloatingWidgetsComponent = CreateDefaultSubobject<UCPlayerWidgetComponent>("FloatingWidgetsComponent");
	FloatingWidgetsComponent->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/ThirdPerson/Maps/_GENERATED/quequ/Sphere_2750B2E7"));
	if (SMFinder.Succeeded()) SplineMesh = SMFinder.Object;
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
	//TextRenderComponent->SetText(FText::FromString(ViewRotation_Delta.ToString()));
	
	TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(MaxWalkSpeed)));
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
	//if (EquippedWeapon_R != nullptr && C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING))
	if (IsEquippingWeapon()  && C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING))
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
	if (C_AnimInstance == nullptr) return;
	C_AnimInstance->SetupDelegates();
	
	if (FloatingWidgetsComponent == nullptr) return;
	FloatingWidgetsComponent->AddFloatingUI(nullptr);
}

void AFOBCharacter::OnRep_MaxWalkSpeed()
{
	ServerSetMaxWalkSpeed(MaxWalkSpeed);
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

void AFOBCharacter::MulticastRootItem_Implementation(AActor* ItemToAdd)
{
	// Weapon
	if (!UKismetSystemLibrary::DoesImplementInterface(ItemToAdd, UWeapon::StaticClass())) return;

	// Server
	if (HasAuthority())
	{
		PossessingWeapons.Add(ItemToAdd);

		// Equip Immediate When No Weapon
		if(!IsEquippingWeapon())
		{
			UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : ServerRootItem_Implementation On %s\t EquippedWeapon_R == nullptr"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
			
			EquippingWeaponsIdx = PossessingWeapons.Num() - 1;
			OnRep_EquippingWeaponsIdx();
		}
		else // Only Add To Inventory
		{
			UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : ServerRootItem_Implementation On %s\t EquippedWeapon_R != nullptr"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
			ItemToAdd->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ItemToAdd->SetActorScale3D(FVector(0.f, 0.f, 0.f));
		}
	}
	else // Client
	{
		FloatingWidgetsComponent->AddFloatingUI(ItemToAdd);
	}
}

void AFOBCharacter::SwitchEquipItem_Implementation(int32 NewEquippingWeaponsIdx)
{
	ServerUnEquipItem();
	EquippingWeaponsIdx = NewEquippingWeaponsIdx;
	OnRep_EquippingWeaponsIdx();
}

void AFOBCharacter::OnThrowGrenadeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : OnThrowGrenadeMontageEnded"));
	GrenadeThrowStep = 0;
	//C_AnimInstance->OnMontageEnded.RemoveDynamic(this, &AFOBCharacter::OnThrowGrenadeMontageEnded);
}

bool AFOBCharacter::IsEquippingWeapon()
{
	return PossessingWeapons.IsValidIndex(EquippingWeaponsIdx);
}

void AFOBCharacter::OnRep_EquippingWeaponsIdx()
{
	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : OnRep_EquippingWeaponsIdx On %s\t"), HasAuthority() ? TEXT("Server") : TEXT("Client"));

	// Weapon Equip
	// Attach Weapon To Character
	if (HasAuthority())
	{
		if (PossessingWeapons.IsValidIndex(EquippingWeaponsIdx))
		{
			PossessingWeapons[EquippingWeaponsIdx]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			PossessingWeapons[EquippingWeaponsIdx]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightHandSocket"));
			PossessingWeapons[EquippingWeaponsIdx]->SetActorScale3D(FVector(1.f, 1.f, 1.f));
		}
	}

	// Play Draw Rifle Anim
	UCPlayerAnimBP* temp_AnimInstance = GetC_AnimInstance();
	if (temp_AnimInstance == nullptr) return;
	C_AnimInstance->PlayRiflePullOut();
}

void AFOBCharacter::OnRep_EquippedWeapon_R()
{
	// Weapon UnEquip
	if (EquippedWeapon_R == nullptr)
	{
		return;
	}

	// Weapon Equip
	// Attach Weapon To Character
	if (HasAuthority())
	{
		EquippedWeapon_R->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("RightHandSocket"));
		PossessingWeapons.Add(EquippedWeapon_R);
	}

	// Play Draw Rifle Anim
	UCPlayerAnimBP* temp_AnimInstance = GetC_AnimInstance();
	if (temp_AnimInstance == nullptr) return;
	C_AnimInstance->PlayRiflePullOut();
}

void AFOBCharacter::OnRep_EquippedWeapon_L()
{
	EquippedWeapon_L->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("LeftHandSocket"));
}

void AFOBCharacter::OnRep_PossessingWeapons()
{
	//Deprecated
}

void AFOBCharacter::AdjustNozzleToAimSpot(float DeltaSeconds)
{
	if (!IsEquippingWeapon())
	{
		UE_LOG(LogTemp, Error, TEXT("AFOBCharacter - AdjustNozzleToAimSpot EquippingWeaponsIdx Is Not Valid Index"));
		return;
	}
	AActor* EquippedWeapon = PossessingWeapons[EquippingWeaponsIdx];
	//IWeapon* I_Weapon = Cast<IWeapon>(EquippedWeapon_R);
	IWeapon* I_Weapon = Cast<IWeapon>(EquippedWeapon);
	if (C_PlayerState == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AFOBCharacter - AdjustNozzleToAimSpot C_PlayerState == nullptr"));
		return;
	}
	if (I_Weapon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AFOBCharacter - AdjustNozzleToAimSpot I_Weapon == nullptr"));
		return;
	}


	//LineTrace Properties
	FHitResult HitResult;
	FVector CameraLocation = (
		C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING) ?
			//Aming
			CameraBoom->GetComponentLocation() :
			//NotAiming
			FollowCamera->GetComponentLocation()
		) + (
			bCrouching ? 
			FVector(0.f, 0.f, -50.f) :
			FVector(0.f, 0.f, 0.f)
			);
	FVector ViewVector = GetViewRotation().Vector();
	float MaxTargetDistance = 5000.f;

	//Weapon Properties
	FVector NozzleLocation = I_Weapon->GetFireSocketPos();
	FVector NozzleVector = EquippedWeapon->GetActorRotation().Vector();
	FCollisionQueryParams CollisionQueryParam = FCollisionQueryParams();
	CollisionQueryParam.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, CameraLocation + ViewVector * MaxTargetDistance, ECC_Pawn, CollisionQueryParam);
	FVector GoalZeroPoint = bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance;

	//DrawDebugSphere(GetWorld(), bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance, 20.f, 32.f, bHit ? FColor::Green : FColor::Red);
	//DrawDebugLine(GetWorld(), CameraLocation, bHit ? HitResult.Location : CameraLocation + ViewVector * MaxTargetDistance, bHit ? FColor::Green : FColor::Red);

	FVector ZeroingPoint(NozzleLocation + NozzleVector * MaxTargetDistance);
	FVector NozzleGoalVector = (GoalZeroPoint - NozzleLocation).GetSafeNormal();
	FRotator DeltaRot = NozzleGoalVector.Rotation() - NozzleVector.Rotation();

	//DrawDebugSphere(GetWorld(), ZeroingPoint, 20.f, 32.f, FColor::Blue);
	//DrawDebugLine(GetWorld(), NozzleLocation, ZeroingPoint, FColor::Blue);

	FRotator CurrWeight = ViewRotation_Delta_Zeroing;

	CurrWeight.Pitch += (3.f + FMath::Atan(FMath::Abs(DeltaRot.Pitch))) * DeltaRot.Pitch / 1.2f * DeltaSeconds;
	CurrWeight.Yaw += (10.f + FMath::Atan(FMath::Abs(DeltaRot.Yaw))) * DeltaRot.Yaw / 1.2f * DeltaSeconds;
	CurrWeight.Pitch = CurrWeight.Pitch > 0 ? FMath::Min(25.f, CurrWeight.Pitch) : FMath::Max(-25.f, CurrWeight.Pitch);
	CurrWeight.Yaw = CurrWeight.Yaw > 0 ? FMath::Min(25.f, CurrWeight.Yaw) : FMath::Max(-25.f, CurrWeight.Yaw);
	CurrWeight.Normalize();

	ServerSetViewRotation_Delta_Zeroing(CurrWeight);
}

void AFOBCharacter::ServerEquipItem_Implementation(AActor* WeaponToEquip)
{
	if (!UKismetSystemLibrary::DoesImplementInterface(WeaponToEquip, UWeapon::StaticClass())) return;

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : ServerEquipItem %s"), *WeaponToEquip->GetName());
	//EquippedWeapon_R = WeaponToEquip;
	//OnRep_EquippedWeapon_R();
}

void AFOBCharacter::ServerUnEquipItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter : ServerUnEquipItem"));

	UCPlayerAnimBP* temp_AnimInstance = GetC_AnimInstance();
	if (temp_AnimInstance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AFOBCharacter : ServerUnEquipItem - UCPlayerAnimBP Not Found"));
		return;
	}
	C_AnimInstance->PlayRiflePutAway();

	PossessingWeapons[EquippingWeaponsIdx]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	PossessingWeapons[EquippingWeaponsIdx]->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PossessingWeapons[EquippingWeaponsIdx]->SetActorScale3D(FVector(0.f, 0.f, 0.f));
	//EquippedWeapon_R = nullptr;
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
	DOREPLIFETIME(AFOBCharacter, ViewRotation_Delta);
	DOREPLIFETIME(AFOBCharacter, ViewRotation_Delta_Zeroing);
	DOREPLIFETIME(AFOBCharacter, bCrouching);
	DOREPLIFETIME(AFOBCharacter, PossessingWeapons);
	DOREPLIFETIME(AFOBCharacter, EquippingWeaponsIdx);
	DOREPLIFETIME(AFOBCharacter, GrenadeAimingDistancePercent);
	DOREPLIFETIME(AFOBCharacter, GrenadeThrowStep);
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
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Ongoing, this, &AFOBCharacter::ShiftTriggered);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AFOBCharacter::ShiftCompleted);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Started, this, &AFOBCharacter::RMBStarted);
		EnhancedInputComponent->BindAction(RMBAction, ETriggerEvent::Completed, this, &AFOBCharacter::RMBCompleted);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AFOBCharacter::CrouchTriggered);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AFOBCharacter::CrouchCompleted);
		EnhancedInputComponent->BindAction(ScrollAction, ETriggerEvent::Triggered, this, &AFOBCharacter::ScrollTriggered);
		EnhancedInputComponent->BindAction(GranadeAction, ETriggerEvent::Started, this, &AFOBCharacter::GrenadeStarted);
		EnhancedInputComponent->BindAction(GranadeAction, ETriggerEvent::Ongoing, this, &AFOBCharacter::GrenadeOnGoing);
		EnhancedInputComponent->BindAction(GranadeAction, ETriggerEvent::Completed, this, &AFOBCharacter::GrenadeCompleted);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFOBCharacter::LMBTriggered_Implementation()
{
	
	//if (EquippedWeapon_R == nullptr)
	if (!IsEquippingWeapon())
	{
		UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - EquippedWeapon_R Not Found"));
		return;
	}

	//IWeapon* I_Weapon = Cast<IWeapon>(EquippedWeapon_R);
	IWeapon* I_Weapon = Cast<IWeapon>(PossessingWeapons[EquippingWeaponsIdx]);
	if (I_Weapon == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - EquippedWeapon_R Not Valid Type"));
		return;
	}

	I_Weapon->LMBTriggered();
}

void AFOBCharacter::RMBStarted_Implementation()
{
	if (C_PlayerState == nullptr) return;
	C_PlayerState->SetPlayerAnimStatus(PLAYER_DASHING, false);
	C_PlayerState->SetPlayerAnimStatus(PLAYER_AIMING, true);

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - RMBTriggered"));
}

void AFOBCharacter::RMBCompleted_Implementation()
{
	if (C_PlayerState == nullptr) return;
	C_PlayerState->SetPlayerAnimStatus(PLAYER_AIMING, false);

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - RMBCompleted"));
}

void AFOBCharacter::ShiftTriggered_Implementation()
{
	if (C_PlayerState == nullptr) return;
	if (C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING)) return;

	C_PlayerState->SetPlayerAnimStatus(PLAYER_DASHING, true);
	//UE_LOG(LogTemp, Log, TEXT("ShiftOnGoing : PLAYER_DASHING Set %s"), C_PlayerState->GetPlayerAnimStatus(PLAYER_DASHING) ? TEXT("True") : TEXT("False"));
}

void AFOBCharacter::ShiftCompleted_Implementation()
{
	if (C_PlayerState == nullptr) return;
	if (C_PlayerState->GetPlayerAnimStatus(PLAYER_AIMING)) return;

	C_PlayerState->SetPlayerAnimStatus(PLAYER_DASHING, false);
	//UE_LOG(LogTemp, Log, TEXT("ShiftCompleted : PLAYER_DASHING Set %s"), C_PlayerState->GetPlayerAnimStatus(PLAYER_DASHING) ? TEXT("True") : TEXT("False"));
}

void AFOBCharacter::SetAimingMode_Implementation(bool e, float DeltaSeconds)
{
	FVector CamCurrLoc = FollowCamera->GetRelativeLocation();
	float ViewRotation_Delta_Pitch = ViewRotation_Delta.Pitch;
	if (ViewRotation_Delta_Pitch > 180.f) ViewRotation_Delta_Pitch -= 360.f;
	CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, e ? CameraBoomLength_FPS : CameraBoomLength_TPS, 0.5f);

	FollowCamera->SetRelativeLocation(
		FVector(
			// X = Camera Move Forward/Backward At Aim Mode
			e ? (
				ViewRotation_Delta_Pitch > 0 ?
				ViewRotation_Delta_Pitch / 45.f * -15.f : ViewRotation_Delta_Pitch / 45.f * -30.f
				) : CamCurrLoc.X,
			// Y = Camera Move Horizontally
			FMath::Lerp(CamCurrLoc.Y, e ? CameraRelativeLocationY_FPS : CameraRelativeLocationY_TPS, 0.5f),
			// Z = Camera Move Up/DownWard At Aim Mode + Crouching Z Axis
			e ? FMath::Abs(ViewRotation_Delta_Pitch / 45.f * 10.f) : CamCurrLoc.Z
		)
	);
	
	
	FVector CurrBoomLoc = CameraBoom->GetRelativeLocation();
	CameraBoom->SetRelativeLocation(
		FVector
		(CurrBoomLoc.X,
			CurrBoomLoc.Y,
			(bCrouching ?
				FMath::Max(CurrBoomLoc.Z - 40.f * DeltaSeconds * 2.f, 20.f) :
				FMath::Min(CurrBoomLoc.Z + 40.f * DeltaSeconds * 2.f, 70.f)
				)
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

void AFOBCharacter::CrouchCompleted_Implementation()
{
	if (C_PlayerState == nullptr) return;
	C_PlayerState->SetPlayerAnimStatus(PLAYER_CROUCH, false);
	bCrouching = false;
	UE_LOG(LogTemp, Log, TEXT("CrouchCompleted : PLAYER_CROUCH Set %s"), C_PlayerState->GetPlayerAnimStatus(PLAYER_CROUCH) ? TEXT("True") : TEXT("False"));
}

void AFOBCharacter::CrouchTriggered_Implementation()
{
	if (C_PlayerState == nullptr) return;
	C_PlayerState->SetPlayerAnimStatus(PLAYER_CROUCH, true);
	bCrouching = true;
	UE_LOG(LogTemp, Log, TEXT("CrouchTriggered : PLAYER_CROUCH Set %s"), C_PlayerState->GetPlayerAnimStatus(PLAYER_CROUCH) ? TEXT("True") : TEXT("False"));
}

void AFOBCharacter::ScrollTriggered_Implementation(const FInputActionValue& Value)
{
	float ScrollAxis = Value.Get<float>();
	ScrollAxis /= FMath::Abs(ScrollAxis); // [ -1, 0, 1]

	if (FloatingWidgetsComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AFOBCharacter - ScrollTriggered_Implementation : FloatingWidgetsComponent Not Found"));
		return;
	}

	int32 CurrEquippingWeaponsIdx = EquippingWeaponsIdx;
	CurrEquippingWeaponsIdx = FMath::FloorToInt(CurrEquippingWeaponsIdx + ScrollAxis) % PossessingWeapons.Num();
	if (CurrEquippingWeaponsIdx < 0) CurrEquippingWeaponsIdx = PossessingWeapons.Num() - 1;
	SwitchEquipItem(CurrEquippingWeaponsIdx);


	FloatingWidgetsComponent->AddFloatingUIArr(ScrollAxis);
	FloatingWidgetsComponent->SetTemporaryVisible();

	UCPlayerAnimBP* temp_AnimInstance = GetC_AnimInstance();
	if (temp_AnimInstance == nullptr) return;
	C_AnimInstance->PlayRiflePullOut();

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - ScrollTriggered_Implementation : %f"), ScrollAxis);
}

void AFOBCharacter::GrenadeStarted_Implementation()
{
	//UCPlayerAnimBP* temp_AnimInstance = GetC_AnimInstance();
	//if (temp_AnimInstance == nullptr) return;
	GrenadeThrowStep = 1;
	//C_AnimInstance->PlayGrenadeThrowInit(); Deprecated : Use BP
	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - GrenadeStarted : %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));	
}

void AFOBCharacter::GrenadeOnGoing_Implementation()
{
	for (USplineMeshComponent* SplinePoint : SplinePoints)
	{
		SplinePoint->DestroyComponent();
	}
	SplinePoints.Empty();

	FHitResult HitResult;
	TArray<FVector> OutPositions;
	FVector Destination;
	TArray<AActor*> IgnoreActor = { this };

	FRotator AimRotator = GetViewRotation();
	AimRotator.Pitch = AimRotator.Pitch > 180.f ? AimRotator.Pitch - 360.f : AimRotator.Pitch;
	GrenadeAimingDistancePercent = (AimRotator.Pitch + 45.f) / 90.f;
	float GrenadeVelocityAcc = 1500.f * GrenadeAimingDistancePercent;

	FVector AimVector = AimRotator.Vector();

	FRotator GrenadeRotator = GetActorRotation();
	GrenadeRotator.Pitch = 30.f;
	UGameplayStatics::Blueprint_PredictProjectilePath_ByTraceChannel(this, HitResult, OutPositions, Destination, GetMesh()->GetSocketLocation("RightHandSocket"), GrenadeRotator.Vector() * GrenadeVelocityAcc, true, 30.f, ECC_Pawn, true, IgnoreActor, EDrawDebugTrace::None, 0.f);

	FVector StartLocation;
	FVector StartTangent;
	FVector EndLocation;
	FVector EndTangent;

	for (int i = 0; i < OutPositions.Num(); i++)
	{
		StartLocation = OutPositions[i];
		StartTangent = i+1 < OutPositions.Num() ? OutPositions[i + 1] - OutPositions[i] : FVector::ZeroVector;
		EndLocation = i+1 < OutPositions.Num() ? OutPositions[i + 1]: StartLocation;
		EndTangent = i+2 < OutPositions.Num() ? OutPositions[i + 2] - OutPositions[i + 1] : FVector::ZeroVector;

		USplineMeshComponent* NewSplinePoint = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		NewSplinePoint->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
		NewSplinePoint->SetStaticMesh(SplineMesh);
		NewSplinePoint->RegisterComponent();
		NewSplinePoint->ForwardAxis = ESplineMeshAxis::Z;
		NewSplinePoint->SetBoundaryMin(-FVector::Dist(StartLocation, EndLocation));
		NewSplinePoint->SetBoundaryMax(FVector::Dist(StartLocation, EndLocation));
		SplinePoints.Add(NewSplinePoint);

		//DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 40.f, FColor::Blue);
	}

	//UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - GrenadeOnGoing : %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
}

void AFOBCharacter::GrenadeCompleted_Implementation()
{
	GrenadeThrowStep = 2;

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - GrenadeCompleted : %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
	for (USplineMeshComponent* SplinePoint : SplinePoints)
	{
		SplinePoint->DestroyComponent();
	}
	SplinePoints.Empty();

	UCPlayerAnimBP* temp_AnimInstance = GetC_AnimInstance();
	if (temp_AnimInstance == nullptr) return;
	FTimerHandle tempTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		tempTimerHandle, FTimerDelegate::CreateLambda(
			[&]
			{
				GrenadeThrowStep = 0;
			}
		), 1.f, false
	);
	//C_AnimInstance->OnMontageEnded.AddDynamic(this, &AFOBCharacter::OnThrowGrenadeMontageEnded);
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
	ViewRotation_Delta = NewViewRotation_Delta;
	OnRep_ViewRotation_Delta();
}


float AFOBCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damaged = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Log, TEXT("AFOBCharacter - Took Damage %f From %s"), DamageAmount, *DamageCauser->GetName());

	return Damaged;
}

UCPlayerAnimBP* AFOBCharacter::GetC_AnimInstance()
{
	if (C_AnimInstance != nullptr) return C_AnimInstance;

	if (GetMesh() == nullptr || GetMesh()->GetAnimInstance() == nullptr) return nullptr;

	C_AnimInstance = Cast<UCPlayerAnimBP>(GetMesh()->GetAnimInstance());
	return C_AnimInstance;
}
