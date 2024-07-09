#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "MonsterCharacter.generated.h"

UINTERFACE(MinimalAPI)
class UMonsterCharacter : public UInterface
{
	GENERATED_BODY()
};

class FOB_API IMonsterCharacter
{
	GENERATED_BODY()

public:

	virtual void SetHP(float NewHP) {}

	virtual float GetHP() { return 0.f; }
	virtual float GetMaxHP() { return 0.f; }
	virtual FVector GetBoneLocation(FName BoneName) { return FVector(); }
};
