#include "Weapon/CLaserRifle.h"
#include "Weapon/CWeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"


ACLaserRifle::ACLaserRifle()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	// Component Setting
	WeaponComponent = CreateDefaultSubobject<UCWeaponComponent>(TEXT("WeaponComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	SetRootComponent(WeaponComponent);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// StaticMesh Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/Resources/Weapons/LaserRifle/XCom_laserRifle_fbx"));
	if (SMFinder.Succeeded()) StaticMeshComponent->SetStaticMesh(SMFinder.Object);
}

void ACLaserRifle::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector ACLaserRifle::GetFireSocketPos()
{
	return StaticMeshComponent->GetSocketLocation("FireSocket");
}

void ACLaserRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACLaserRifle::LMBTriggered_Implementation()
{
	if (WeaponComponent == nullptr) return;

	FVector FirePos = StaticMeshComponent->GetSocketLocation("FireSocket");

	//UE_LOG(LogTemp, Log, TEXT("%s\tACXM177 Firesocket- LMBTriggered At %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"), *FirePos.ToString());
	//UE_LOG(LogTemp, Log, TEXT("%s\tACXM177 StaticMesh- LMBTriggered At %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"), *GetActorLocation().ToString());

	WeaponComponent->Fire(
		FirePos,
		nullptr,
		1.f
	);
}

