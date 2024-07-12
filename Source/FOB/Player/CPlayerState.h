// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CPlayerState.generated.h"

/**
 * 
 */
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
	UPROPERTY(Replicated)
	FRotator ViewRotation_Delta;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int32 PlayerAnimStatus;
public:
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

	UFUNCTION(Server, Reliable)
	void SetViewRotation_Delta(FRotator NewViewRotation_Delta);
	FRotator GetViewRotation_Delta() { return ViewRotation_Delta; }

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

};
