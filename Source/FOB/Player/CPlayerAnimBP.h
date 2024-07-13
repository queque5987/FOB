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

public:

	UFUNCTION(BlueprintCallable)
	void UpdateProperties(float DeltaSeconds);

	UPROPERTY(BlueprintReadOnly)
	class AFOBCharacter* OwningPlayer;

	UPROPERTY(BlueprintReadOnly)
	FVector2D MovementVector;

	UPROPERTY(BlueprintReadOnly)
	FRotator ViewRotation_Delta;

	void SetViewRotation_Delta(FRotator NewViewRotation_Delta);
};
