// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/CWeaponComponent.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"


// Sets default values for this component's properties
UCWeaponComponent::UCWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetSphereRadius(200.f);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Collider->SetupAttachment(this);
}


// Called when the game starts
void UCWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &UCWeaponComponent::OnComponentBeginOverlap);
}


void UCWeaponComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPlayerCharacter* I_PlayerCharacter = Cast<IPlayerCharacter>(OtherActor);
	if (I_PlayerCharacter == nullptr) return;

	I_PlayerCharacter->ServerEquipItem(GetOwner());
}

// Called every frame
void UCWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

