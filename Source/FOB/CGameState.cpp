// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameState.h"
#include "Monster/MonsterCharacter.h"

ACGameState::ACGameState() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACGameState::Tick(float DeltaTime)
{
}

void ACGameState::MonsterDied(APawn* SpawnedMonster)
{
	UE_LOG(LogTemp, Log, TEXT("Monster %s Died"), *SpawnedMonster->GetName());
}

float ACGameState::AddSpawnedMonsterHP(APawn* SpawnedMonster, float MaxHP)
{
	if (SpawnedMonsterStateMap.Contains(SpawnedMonster))
	{
		return GetSpawnedMonsterHP(SpawnedMonster);
	}
	return SpawnedMonsterStateMap.Add(SpawnedMonster, MaxHP);
}

float ACGameState::TakeDamageSpawnedMonster(APawn* SpawnedMonster, float DamageAmount)
{
	//SpawnedMonsterStateMap[SpawnedMonster] -= DamageAmount;
	SpawnedMonsterStateMap[SpawnedMonster] = GetSpawnedMonsterHP(SpawnedMonster) - DamageAmount;
	return *SpawnedMonsterStateMap.Find(SpawnedMonster);
}

float ACGameState::GetSpawnedMonsterHP(APawn* SpawnedMonster)
{
	float* CurrHP = SpawnedMonsterStateMap.Find(SpawnedMonster);
	if (CurrHP == nullptr) return -1.f;
	if (*CurrHP <= 0.f) MonsterDied(SpawnedMonster);
	return *SpawnedMonsterStateMap.Find(SpawnedMonster);
}
