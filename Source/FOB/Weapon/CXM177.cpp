#include "Weapon/CXM177.h"
#include "Weapon/CWeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Net/UnrealNetwork.h"

ACXM177::ACXM177() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	// Component Setting
	WeaponComponent = CreateDefaultSubobject<UCWeaponComponent>(TEXT("WeaponComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent_Magazine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Magazine"));
	//StaticMeshComponent->SetIsReplicated(true);

	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent_Magazine->SetupAttachment(RootComponent);
	WeaponComponent->SetupAttachment(RootComponent);

	// StaticMesh Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/Resources/Weapons/xm177/xm177_fbx_2018"));
	if (SMFinder.Succeeded()) StaticMeshComponent->SetStaticMesh(SMFinder.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SM_MagFinder(TEXT("/Game/Resources/Weapons/xm177/30roundmagazine_fbx_2018"));
	if (SM_MagFinder.Succeeded()) StaticMeshComponent_Magazine->SetStaticMesh(SM_MagFinder.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BulletFinder(TEXT("/Game/Resources/Weapons/xm177/556_cartridge_fbx_2018_Projectile"));
	if (SM_BulletFinder.Succeeded()) BulletStaticMesh = SM_BulletFinder.Object;
}

void ACXM177::BeginPlay()
{
	Super::BeginPlay();
}

void ACXM177::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FVector ACXM177::GetFireSocketPos()
{
	return StaticMeshComponent->GetSocketLocation("FireSocket");
}

void ACXM177::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ServerFireReady();
}

void ACXM177::LMBTriggered_Implementation()
{
	if (WeaponComponent == nullptr) return;

	FVector FirePos = StaticMeshComponent->GetSocketLocation("FireSocket");

	//UE_LOG(LogTemp, Log, TEXT("%s\tACXM177 Firesocket- LMBTriggered At %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"), *FirePos.ToString());
	//UE_LOG(LogTemp, Log, TEXT("%s\tACXM177 StaticMesh- LMBTriggered At %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"), *GetActorLocation().ToString());

	WeaponComponent->Fire(
		FirePos,
		BulletStaticMesh,
		1.f
	);
}
