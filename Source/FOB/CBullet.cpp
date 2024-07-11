#include "CBullet.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Monster/CMonsterCharacter.h"
#include "Engine/DamageEvents.h"
#include "PCH.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Player/UIController.h"

ACBullet::ACBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	Collider = CreateDefaultSubobject<USphereComponent>("Collider");

	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/Megascans/3D_Assets/Potato_vfdlbdrjw/S_Potato_vfdlbdrjw_lod3_Var1.S_Potato_vfdlbdrjw_lod3_Var1"));
	if (SMFinder.Succeeded()) StaticMeshComponent->SetStaticMesh(SMFinder.Object);

	Collider->SetSphereRadius(30.f);

	fClock = 0.f;
	Acc = 500.f;
	bReplicates = true;
	DamageType = UDamageType::StaticClass();

	StaticMeshComponent->SetRelativeScale3D(FVector(10.f, 10.f, 10.f));

	SetRootComponent(StaticMeshComponent);
	Collider->SetupAttachment(StaticMeshComponent);
}

void ACBullet::BeginPlay()
{
	Super::BeginPlay();
}

void ACBullet::CheckOverlapingActor()
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

	if (!bHit) return;

	for (const FHitResult HR : HitResults)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HR.GetActor()->GetName());
		ACMonsterCharacter* Monster = Cast<ACMonsterCharacter>(HR.GetActor());
		if (Monster == nullptr) continue;

		if (OwningPlayer->HasAuthority())
		{
			UGameplayStatics::ApplyPointDamage(Monster, 1.f, Monster->GetActorLocation() - GetActorLocation(), HR, OwningPlayer->GetController(), OwningPlayer, DamageType);
		}
		else
		{
			IUIController* UIController = Cast<IUIController>(OwningPlayer->GetController());
			if (UIController == nullptr) continue;
			UIController->SetDealingMonsterCharacter(Monster);
		}

		Destroy();
	}

	//DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 32.f, bHit? FColor::Green : FColor::Red, false, 0.2f);

}

void ACBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	fClock += DeltaTime;

	if (fClock > 1.2f) Destroy();
	else
	{
		FVector NextTickLocation = GetActorLocation() + GetActorForwardVector() * (Acc * DeltaTime);
		SetActorLocation(NextTickLocation);
	}

	CheckOverlapingActor();
}
