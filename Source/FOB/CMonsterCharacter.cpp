#include "CMonsterCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "PCH.h"
#include "UIController.h"
#include "CGameState.h"
#include "Net/UnrealNetwork.h"
#include "CMonsterAnimBP.h"
#include "CMonsterAIController.h"
#include "Components/ShapeComponent.h"
#include "MonsterMovementComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"

ACMonsterCharacter::ACMonsterCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UMonsterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/Meshes/SK_MOUNTAIN_DRAGON.SK_MOUNTAIN_DRAGON"));
	if (SKFinder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SKFinder.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
	GetMesh()->SetupAttachment(RootComponent);

	MaxHP = 100.f;
	fHP = MaxHP;

	TextRenderComponent->SetupAttachment(RootComponent);
	TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(fHP)));

	GetMesh()->SetCollisionObjectType(COLLISION_CHANNEL_MONSTER);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetMesh()->SetCollisionResponseToChannel(COLLISION_CHANNEL_BULLET, ECollisionResponse::ECR_Overlap);

	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/BP_MonsterAnimBP"));
	if (AnimBPFinder.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPFinder.Class);
	}

	AIControllerClass = ACMonsterAIController::StaticClass();

	ConstructorHelpers::FObjectFinder<UAnimSequence> BiteAnimFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/Animations/Hostile/ANIM_MOUNTAIN_DRAGON_bite"));
	if (BiteAnimFinder.Succeeded()) BiteAnimSequence = BiteAnimFinder.Object;

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void ACMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	ACGameState* GameState = Cast<ACGameState>(GetWorld()->GetGameState());
	if (GameState == nullptr) return;
	fHP = GameState->AddSpawnedMonsterHP(this, fHP);

	SetBindDelegates();
}

void ACMonsterCharacter::UpdateMonsterHP_Implementation()
{
	ACGameState* GameState = Cast<ACGameState>(GetWorld()->GetGameState());
	if (GameState == nullptr) return;
	fHP = GameState->GetSpawnedMonsterHP(this);
}

void ACMonsterCharacter::DamageMonster_Implementation(float DamageAmount)
{
	ACGameState* GameState = Cast<ACGameState>(GetWorld()->GetGameState());
	if (GameState == nullptr) return;
	GameState->TakeDamageSpawnedMonster(this, DamageAmount);
}

void ACMonsterCharacter::SetBindDelegates_Implementation()
{
	DoAttack.BindUFunction(this, FName("ServerDoAttack"));
}

void ACMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMonsterHP();

	//GetMesh()->GetBoneLocation("MOUNTAIN_DRAGON_-Head");
	//if (HasAuthority()) DrawDebugSphere(GetWorld(), GetMesh()->GetBoneLocation("MOUNTAIN_DRAGON_-Head"), 120.f, 32.f, FColor::Blue);

	if (TextRenderComponent == nullptr) return;
	TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(fHP)));
}

float ACMonsterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float TotalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageMonster(DamageAmount);

	IUIController* UIController = Cast<IUIController>(EventInstigator);
	if (UIController != nullptr)
	{
		UIController->SetDealingMonsterCharacter(this);
	}

	UE_LOG(LogTemp, Log, TEXT("Dealt Damage %f"), DamageAmount);

	return TotalDamageAmount;
}

void ACMonsterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACMonsterCharacter, fHP);
	DOREPLIFETIME(ACMonsterCharacter, BiteAnimSequence);
}

void ACMonsterCharacter::SetHP_Implementation(float NewHP)
{
	fHP = NewHP;
}

FVector ACMonsterCharacter::GetBoneLocation(FName BoneName)
{
	return GetMesh()->GetBoneLocation("MOUNTAIN_DRAGON_-Head");
}


bool ACMonsterCharacter::ServerDoAttack_Validate(const FString& AttackType)
{
	return true;
}

void ACMonsterCharacter::ServerDoAttack_Implementation(const FString& AttackType)
{
	if (BiteAnimSequence == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("ClientDoAttack_Implementation : %s Can Not Be Found"), *AttackType);
		return;
	}
	GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(BiteAnimSequence, "DefaultSlot");

	MulticastDoAttack(AttackType);
}

void ACMonsterCharacter::MulticastDoAttack_Implementation(const FString& AttackType)
{
	//UE_LOG(LogTemp, Log, TEXT("ClientDoAttack_Implementation : Playing %s"), *AttackType);
	if (BiteAnimSequence == nullptr)
	{
		//UE_LOG(LogTemp, Log, TEXT("ClientDoAttack_Implementation : %s Can Not Be Found"), *AttackType);
		return;
	}
	GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(BiteAnimSequence, "DefaultSlot");
}

