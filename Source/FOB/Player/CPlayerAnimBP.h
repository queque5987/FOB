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

	class AFOBCharacter* OwningPlayer;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void UpdateProperties(float DeltaSeconds);
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector2D MovementVector;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FRotator ViewRotation_Delta;

	UFUNCTION(Server, Reliable)
	void SetViewRotation_Delta(FRotator NewViewRotation_Delta);
};
