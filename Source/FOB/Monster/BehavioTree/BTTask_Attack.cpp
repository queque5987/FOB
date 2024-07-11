// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "Monster/CMonsterAIController.h"
#include "Monster/CMonsterCharacter.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACMonsterCharacter* MonsterCharacter = Cast<ACMonsterCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (MonsterCharacter != nullptr)
	{
		return MonsterCharacter->DoAttack.ExecuteIfBound(AttackType) ?
			EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
