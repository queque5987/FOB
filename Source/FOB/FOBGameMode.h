// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FOBGameMode.generated.h"

UCLASS(minimalapi)
class AFOBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFOBGameMode();

	void SpawnPotato(FVector SpawnLocation, FRotator SpawnRotation);
};



