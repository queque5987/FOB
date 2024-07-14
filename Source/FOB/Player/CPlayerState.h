// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CPlayerState.generated.h"

//DECLARE_DELEGATE_OneParam(FPlayerAnimStatusUpdated, int32);

UCLASS()
class FOB_API ACPlayerState : public APlayerState
{
	GENERATED_BODY()

	ACPlayerState();

	virtual void BeginPlay() override;
	UPROPERTY(Replicated)
	float MaxHP;
	UPROPERTY(Replicated)
	float fHP;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_PlayerAnimStatus)
	int32 PlayerAnimStatus;
	UFUNCTION()
	void OnRep_PlayerAnimStatus();
public:
	//FPlayerAnimStatusUpdated PlayerAnimStatusUpdated;

	UFUNCTION(NetMulticast, Reliable)
	void SetupDelegates();

	UFUNCTION(Client, Reliable)
	void SetMaxHP(float NewMaxHP);
	UFUNCTION(Client, Reliable)
	void SetHP(float NewHP);

	UFUNCTION(Server, Reliable)
	void TakeAwayHP(float DamageAmount);

	float GetMaxHP() { return MaxHP; }
	float GetHP() { return fHP; }

	UFUNCTION(Server, Reliable)
	void SetPlayerAnimStatus(int32 PlayerAnimStatusType, bool e);

	bool GetPlayerAnimStatus(int32 PlayerAnimStatusType);

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

};
