#include "Weapon/CXM177.h"
#include "Weapon/CWeaponComponent.h"
#include "Components/StaticMeshComponent.h"

ACXM177::ACXM177() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	// Component Setting
	WeaponComponent = CreateDefaultSubobject<UCWeaponComponent>(TEXT("WeaponComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent_Magazine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Magazine"));

	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent_Magazine->SetupAttachment(RootComponent);
	WeaponComponent->SetupAttachment(RootComponent);
	// StaticMesh Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/Resources/Weapons/xm177/xm177_fbx_2018"));
	if (SMFinder.Succeeded()) StaticMeshComponent->SetStaticMesh(SMFinder.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SM_MagFinder(TEXT("/Game/Resources/Weapons/xm177/30roundmagazine_fbx_2018"));
	if (SM_MagFinder.Succeeded()) StaticMeshComponent_Magazine->SetStaticMesh(SM_MagFinder.Object);
}

void ACXM177::BeginPlay()
{
	Super::BeginPlay();
}

void ACXM177::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

