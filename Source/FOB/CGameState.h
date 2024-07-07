// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CGameState.generated.h"

/**
 * 
 */
UCLASS()
class FOB_API ACGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	ACGameState();

	TMap<class APawn*, float> SpawnedMonsterStateMap;


	virtual void Tick(float DeltaTime) override;
public:

	float AddSpawnedMonsterHP(APawn* SpawnedMonster, float MaxHP);
	float TakeDamageSpawnedMonster(APawn* SpawnedMonster, float DamageAmount);
	float GetSpawnedMonsterHP(APawn* SpawnedMonster);
};
