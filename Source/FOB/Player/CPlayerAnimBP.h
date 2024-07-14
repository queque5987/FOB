// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CPlayerAnimBP.generated.h"

UCLASS()
class FOB_API UCPlayerAnimBP : public UAnimInstance
{
	GENERATED_BODY()
	
	UCPlayerAnimBP();

	virtual void NativeInitializeAnimation() override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	class ACPlayerState* C_PlayerState;
public:
	UFUNCTION(NetMulticast, Reliable)
	void SetupDelegates();

	UFUNCTION()
	void SetbCrouching(bool e);

	UFUNCTION(Server, Reliable)
	void ServerSetbCrouching(bool e);

	UFUNCTION(Client, Reliable)
	void ClientSetbCrouching(bool e);

	UFUNCTION(BlueprintCallable)
	void UpdateProperties(float DeltaSeconds);

	//UFUNCTION()
	//void OnPlayerAnimStatusUpdated(int32 PlayerAnimStatus);

	UPROPERTY(BlueprintReadOnly)
	class AFOBCharacter* OwningPlayer;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MovementVector;

	UPROPERTY(BlueprintReadOnly)
	FRotator ViewRotation_Delta;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCrouching;

	void SetViewRotation_Delta(FRotator NewViewRotation_Delta);
};
