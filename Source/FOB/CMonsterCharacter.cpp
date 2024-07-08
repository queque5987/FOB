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

	//SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	//MonsterMovementComponent = CreateDefaultSubobject<UMonsterMovementComponent>("MonsterMovementComponent");

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
	GetMesh()->SetCollisionResponseToChannel(COLLISION_CHANNEL_BULLET, ECollisionResponse::ECR_Overlap);

	ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/BP_MonsterAnimBP"));
	if (AnimBPFinder.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPFinder.Class);
	}

	AIControllerClass = ACMonsterAIController::StaticClass();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	FARFilter Filter;
	Filter.PackagePaths.Add("/Game/Resources/QuadrapedCreatures/MountainDragon/Animations/Hostile");
	AssetRegistryModule.Get().GetAssets(Filter, AssetData);
	UAnimSequence* tempAnim;
	for (FAssetData Dat : AssetData)
	{
		tempAnim = Cast<UAnimSequence>(Dat.GetAsset());
		if (tempAnim == nullptr) continue;
		HostileAnimMap.Add(Dat.AssetName.ToString(), tempAnim);
		UE_LOG(LogTemp, Log, TEXT("Loading Asset : %s"), *Dat.AssetName.ToString());
	}
}

void ACMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	ACGameState* GameState = Cast<ACGameState>(GetWorld()->GetGameState());
	if (GameState == nullptr) return;
	fHP = GameState->AddSpawnedMonsterHP(this, fHP);

	DoAttack.BindUFunction(this, FName("ServerDoAttack"));
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

void ACMonsterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMonsterHP();

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


	return TotalDamageAmount;
}

void ACMonsterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACMonsterCharacter, fHP);
}

void ACMonsterCharacter::SetHP_Implementation(float NewHP)
{
	//UE_LOG(LogTemp, Log, TEXT("Curr HP : %f >> New HP : %f"), fHP, NewHP);
	fHP = NewHP;
}

void ACMonsterCharacter::ServerDoAttack_Implementation(const FString& AttackType)
{
	ClientDoAttack(AttackType);
}

void ACMonsterCharacter::ClientDoAttack_Implementation(const FString& AttackType)
{
	UAnimSequence* tempAnimsequence = *HostileAnimMap.Find(AttackType);
	if (tempAnimsequence == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("ClientDoAttack_Implementation : %s Can Not Be Found"), *AttackType);
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("ClientDoAttack_Implementation : Playing %s"), *AttackType);

	GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(tempAnimsequence, "DefaultSlot");
}

