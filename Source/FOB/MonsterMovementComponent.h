#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonsterMovementComponent.generated.h"

UCLASS()
class FOB_API UMonsterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	
	UMonsterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	
};
