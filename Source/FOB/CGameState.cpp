// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameState.h"
#include "MonsterCharacter.h"

ACGameState::ACGameState() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACGameState::Tick(float DeltaTime)
{
}

float ACGameState::AddSpawnedMonsterHP(APawn* SpawnedMonster, float MaxHP)
{
	if (SpawnedMonsterStateMap.Contains(SpawnedMonster))
	{
		//UE_LOG(LogTemp, Log, TEXT("AddSpawnedMonsterHP - Contains - %s"), *SpawnedMonster->GetName());
		return GetSpawnedMonsterHP(SpawnedMonster);
	}
	//UE_LOG(LogTemp, Log, TEXT("CreateHUD_Implementation - Add To Map - %s"), *SpawnedMonster->GetName());
	return SpawnedMonsterStateMap.Add(SpawnedMonster, MaxHP);
}

float ACGameState::TakeDamageSpawnedMonster(APawn* SpawnedMonster, float DamageAmount)
{
	SpawnedMonsterStateMap[SpawnedMonster] -= DamageAmount;
	return *SpawnedMonsterStateMap.Find(SpawnedMonster);
}

float ACGameState::GetSpawnedMonsterHP(APawn* SpawnedMonster)
{
	//UE_LOG(LogTemp, Log, TEXT("GetSpawnedMonsterHP - Returning Value%f"), *SpawnedMonsterStateMap.Find(SpawnedMonster));

	return *SpawnedMonsterStateMap.Find(SpawnedMonster);
}
