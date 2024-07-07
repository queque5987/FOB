#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "UIController.generated.h"

UINTERFACE(MinimalAPI)
class UUIController : public UInterface
{
	GENERATED_BODY()
};

class FOB_API IUIController
{
	GENERATED_BODY()

public:
	
	virtual void SetDealingMonsterCharacter(class ACMonsterCharacter* DealingMonsterCharacter) {};
};
