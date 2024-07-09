// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "FOBCharacter.h"

ACPlayerState::ACPlayerState() : Super()
{
}

void ACPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ACPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayerState, fHP);
	DOREPLIFETIME(ACPlayerState, MaxHP);
}

void ACPlayerState::SetMaxHP_Implementation(float NewMaxHP)
{
	MaxHP = NewMaxHP;
}

void ACPlayerState::SetupDelegates_Implementation()
{
	MaxHP = 100.f;
	fHP = MaxHP;

	AController* Controller = GetPlayerController();
	if (Controller == nullptr) return;

	APawn* OwningActor = GetPlayerController()->GetPawn();

	if (OwningActor == nullptr) return;

	UE_LOG(LogTemp, Log, TEXT("ACPlayerState - SetupDelegates"));

	OwningActor->OnTakeAnyDamage.AddDynamic(this, &ACPlayerState::OnTakeDamage);
}

void ACPlayerState::SetHP_Implementation(float NewHP)
{
	fHP = NewHP;
}

void ACPlayerState::TakeAwayHP_Implementation(float DamageAmount)
{
	fHP -= DamageAmount;

	//UE_LOG(LogTemp, Log, TEXT("ACPlayerState - TakeAwayHP %f"), DamageAmount);
	UE_LOG(LogTemp, Log, TEXT("ACPlayerState - NewHP %f"), fHP);
}

void ACPlayerState::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	TakeAwayHP(Damage);

	UE_LOG(LogTemp, Log, TEXT("ACPlayerState - OnTakeDamage"));
}
