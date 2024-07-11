#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "PlayerCharacter.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerCharacter : public UInterface
{
	GENERATED_BODY()
};

class FOB_API IPlayerCharacter
{
	GENERATED_BODY()

public:
	
	virtual void ServerEquipItem(class AActor* WeaponToEquip) {};
	virtual void ServerUnEquipItem() {};
};
