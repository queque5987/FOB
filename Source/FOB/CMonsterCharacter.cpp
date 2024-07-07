#include "CMonsterCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "PCH.h"
#include "UIController.h"
#include "CGameState.h"
#include "Net/UnrealNetwork.h"

ACMonsterCharacter::ACMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SKeletalMeshComponent"));
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SKFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/Meshes/SK_MOUNTAIN_DRAGON.SK_MOUNTAIN_DRAGON"));
	if (SKFinder.Succeeded())
	{
		//GetMesh()->SetSkeletalMesh(SKFinder.Object);
		SkeletalMeshComponent->SetSkeletalMesh(SKFinder.Object);
	}

	SetRootComponent(SkeletalMeshComponent);

	MaxHP = 100.f;
	fHP = MaxHP;

	TextRenderComponent->SetupAttachment(SkeletalMeshComponent);
	TextRenderComponent->SetText(FText::FromString(FString::SanitizeFloat(fHP)));

	SkeletalMeshComponent->SetCollisionObjectType(COLLISION_CHANNEL_MONSTER);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMeshComponent->SetCollisionResponseToChannel(COLLISION_CHANNEL_BULLET, ECollisionResponse::ECR_Overlap);

}

void ACMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	ACGameState* GameState = Cast<ACGameState>(GetWorld()->GetGameState());
	if (GameState == nullptr) return;
	fHP = GameState->AddSpawnedMonsterHP(this, fHP);
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
	UE_LOG(LogTemp, Log, TEXT("Curr HP : %f >> New HP : %f"), fHP, NewHP);
	fHP = NewHP;
}

