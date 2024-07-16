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

	UAnimSequence* RiflePullOut;
	UAnimSequence* RiflePutAway;
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

//Player Properties
	UPROPERTY(BlueprintReadOnly)
	class AFOBCharacter* OwningPlayer;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MovementVector;

	UPROPERTY(BlueprintReadOnly)
	FRotator ViewRotation_Delta;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCrouching;

	//UPROPERTY(Replicated, BlueprintReadOnly)
	//bool bLeftHandFull;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bRightHandFull;
//Player Properties End

//Play Montage

	UFUNCTION()
	void PlayRiflePullOut();

	UFUNCTION()
	void PlayRiflePutAway();
//Play Montage End

	void SetViewRotation_Delta(FRotator NewViewRotation_Delta);
};
