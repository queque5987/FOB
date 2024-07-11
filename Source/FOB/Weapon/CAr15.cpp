#include "Weapon/CAr15.h"
#include "Weapon/CWeaponComponent.h"
#include "Components/StaticMeshComponent.h"

ACAr15::ACAr15() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponComponent = CreateDefaultSubobject<UCWeaponComponent>(TEXT("WeaponComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));


}

void ACAr15::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACAr15::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

