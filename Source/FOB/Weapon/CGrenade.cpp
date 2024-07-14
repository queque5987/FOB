#include "Weapon/CGrenade.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Monster/CMonsterCharacter.h"
#include "Engine/DamageEvents.h"
#include "PCH.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Player/UIController.h"


ACGrenade::ACGrenade()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	// Component Setting
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	// StaticMesh Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/Megascans/3D_Assets/Potato_vfdlbdrjw/S_Potato_vfdlbdrjw_lod3_Var1"));
	if (SMFinder.Succeeded()) StaticMeshComponent->SetStaticMesh(SMFinder.Object);

	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void ACGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACGrenade::CheckOverlapingActor()
{
	ACharacter* OwningPlayer = Cast<ACharacter>(GetOwner());
	if (OwningPlayer == nullptr) return;

	FHitResult HitResult;
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams tempParam;
	FCollisionShape tempCollisionShpae;

	FCollisionQueryParams tempQueryParam;
	tempQueryParam.AddIgnoredActor(this);

	tempParam.AddObjectTypesToQuery(COLLISION_CHANNEL_MONSTER);
	tempCollisionShpae.SetSphere(30.f);

	bool bHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetActorLocation(),
		GetActorLocation(),
		FQuat::Identity,
		tempParam,
		tempCollisionShpae
	);

	FDamageEvent DamageEvent = FDamageEvent();

	if (!bHit)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 32.f, bHit ? FColor::Green : FColor::Red, false, 0.2f);

	for (const FHitResult HR : HitResults)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HR.GetActor()->GetName());
		ACMonsterCharacter* Monster = Cast<ACMonsterCharacter>(HR.GetActor());
		if (Monster == nullptr) continue;

		if (OwningPlayer->HasAuthority())
		{
			UGameplayStatics::ApplyPointDamage(Monster, 1.f, Monster->GetActorLocation() - GetActorLocation(), HR, OwningPlayer->GetController(), OwningPlayer, NULL);
		}
		else
		{
			IUIController* UIController = Cast<IUIController>(OwningPlayer->GetController());
			if (UIController == nullptr) continue;
			UIController->SetDealingMonsterCharacter(Monster);
		}

		Destroy();
		break;
	}
}

void ACGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckOverlapingActor();
}

