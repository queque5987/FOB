// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CMonsterAIController.generated.h"


UCLASS()
class FOB_API ACMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
	ACMonsterAIController(const FObjectInitializer& ObjectInitializer);
	
	class UBehaviorTree* BTAsset;
	class UBlackboardData* BBAsset;

	virtual void OnPossess(APawn* InPawn) override;

public:
	static const FName HomePosKey;
	static const FName PatrolPosKey;

};
