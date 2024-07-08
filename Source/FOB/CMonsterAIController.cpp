// Fill out your copyright notice in the Description page of Project Settings.


#include "CMonsterAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName ACMonsterAIController::HomePosKey(TEXT("HomePos"));
const FName ACMonsterAIController::PatrolPosKey(TEXT("PatrolPos"));

ACMonsterAIController::ACMonsterAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree>BTAssetFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/BT_Monster"));
	ConstructorHelpers::FObjectFinder<UBlackboardData>BBAssetFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/BB_Monster"));
	if (BBAssetFinder.Succeeded() && BTAssetFinder.Succeeded())
	{
		BTAsset = BTAssetFinder.Object;
		BBAsset = BBAssetFinder.Object;
	}
}

void ACMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BBAsset == nullptr || BTAsset == nullptr) return;

	UBlackboardComponent* BB = Blackboard;
	if (!UseBlackboard(BBAsset, BB)) return;
	if (!RunBehaviorTree(BTAsset)) return;

	Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
}

