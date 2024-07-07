// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CMonsterAnimBP.generated.h"

/**
 * 
 */
UCLASS()
class FOB_API UCMonsterAnimBP : public UAnimInstance
{
	GENERATED_BODY()
	
	UCMonsterAnimBP();
	
	class APawn* Pawn;

	virtual void NativeInitializeAnimation() override;

public:

	UFUNCTION(BlueprintCallable)
	void UpdateProperties(float Delta);

	UPROPERTY(BlueprintReadOnly)
	float PawnVelocity;
	UPROPERTY(BlueprintReadOnly)
	float PawnMaxVelocity;
};
