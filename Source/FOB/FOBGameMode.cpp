// Copyright Epic Games, Inc. All Rights Reserved.

#include "FOBGameMode.h"
#include "FOBCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "CBullet.h"
#include "CPlayerController.h"
#include "CPlayerState.h"
#include "CGameState.h"

AFOBGameMode::AFOBGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PlayerControllerClass = ACPlayerController::StaticClass();
	PlayerStateClass = ACPlayerState::StaticClass();
	GameStateClass = ACGameState::StaticClass();
}

void AFOBGameMode::SpawnPotato(FVector SpawnLocation, FRotator SpawnRotation)
{
	UE_LOG(LogTemp, Log, TEXT("Spawn Location : %s"), *SpawnLocation.ToString());
	GetWorld()->SpawnActor<ACBullet>(ACBullet::StaticClass(), SpawnLocation, SpawnRotation);
}
