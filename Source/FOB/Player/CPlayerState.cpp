// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Player/FOBCharacter.h"

ACPlayerState::ACPlayerState() : Super()
{
	PlayerAnimStatus = 0;
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
	DOREPLIFETIME(ACPlayerState, PlayerAnimStatus);
}

void ACPlayerState::OnRep_PlayerAnimStatus()
{
	UE_LOG(LogTemp, Log, TEXT("ACPlayerState - OnRep_PlayerAnimStatus %d"), PlayerAnimStatus);
	//if (PlayerAnimStatusUpdated.IsBound())
	//{
	//	PlayerAnimStatusUpdated.Execute(PlayerAnimStatus);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Log, TEXT("ACPlayerState - OnRep_PlayerAnimStatus Execute Failed"));
	//}
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

	UE_LOG(LogTemp, Log, TEXT("ACPlayerState - NewHP %f"), fHP);
}

void ACPlayerState::SetPlayerAnimStatus_Implementation(int32 PlayerAnimStatusType, bool e)
{
	if (e != GetPlayerAnimStatus(PlayerAnimStatusType)) PlayerAnimStatus += e ? PlayerAnimStatusType : -PlayerAnimStatusType;
}

bool ACPlayerState::GetPlayerAnimStatus(int32 PlayerAnimStatusType)
{
	return (PlayerAnimStatus & PlayerAnimStatusType);
}

void ACPlayerState::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	TakeAwayHP(Damage);

	UE_LOG(LogTemp, Log, TEXT("ACPlayerState - OnTakeDamage"));
}
