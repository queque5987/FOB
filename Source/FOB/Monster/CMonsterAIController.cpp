// Fill out your copyright notice in the Description page of Project Settings.


#include "CMonsterAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/FOBCharacter.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"

const FName ACMonsterAIController::HomePosKey(TEXT("HomePos"));
const FName ACMonsterAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName ACMonsterAIController::PlayerPosKey(TEXT("PlayerPos"));
const FName ACMonsterAIController::bSeePlayerKey(TEXT("bSeePlayer"));
const FName ACMonsterAIController::bHostileKey(TEXT("bHostile"));

ACMonsterAIController::ACMonsterAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree>BTAssetFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/BT_Monster"));
	ConstructorHelpers::FObjectFinder<UBlackboardData>BBAssetFinder(TEXT("/Game/Resources/QuadrapedCreatures/MountainDragon/BB_Monster"));
	if (BBAssetFinder.Succeeded() && BTAssetFinder.Succeeded())
	{
		BTAsset = BTAssetFinder.Object;
		BBAsset = BBAssetFinder.Object;
	}
	SetPerceptionSystem();
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

void ACMonsterAIController::SetPerceptionSystem()
{
	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightConfig->SightRadius = 6000.f;
	SightConfig->LoseSightRadius = 2000.f;
	SightConfig->PeripheralVisionAngleDegrees = 120.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 900.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ACMonsterAIController::OnTargetDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void ACMonsterAIController::OnTargetDetected(AActor* actor, FAIStimulus const Stimulus)
{
	if (auto const player = Cast<AFOBCharacter>(actor))
	{
		Blackboard->SetValueAsBool(bSeePlayerKey, Stimulus.WasSuccessfullySensed());
		Blackboard->SetValueAsVector(PlayerPosKey, player->GetActorLocation());

		if (Stimulus.WasSuccessfullySensed())
		{
			//GetWorld()->GetTimerManager().ClearTimer(PlayerLoseTimerHandle);
			//ChasingPlayer = player;

			//player->SetLastDealingEnemy(EC);

			//EC->SetbHostile(true);
			Blackboard->SetValueAsBool(bHostileKey, true);
			Blackboard->SetValueAsVector(PatrolPosKey, GetPawn()->GetActorLocation());
		}
		//else
		//{
		//	//Blackboard->SetValueAsObject(PlayerCharacter, nullptr);
		//	GetWorld()->GetTimerManager().SetTimer(PlayerLoseTimerHandle, this, &ACEnemyAIController::OnPlayerLoseTimer, PlayerLoseInterval);
		//}
	}
}

