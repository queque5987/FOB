// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapon.generated.h"

UINTERFACE(MinimalAPI)
class UWeapon : public UInterface
{
	GENERATED_BODY()
};

class FOB_API IWeapon
{
	GENERATED_BODY()

protected:
	virtual void GetFireSocketPos(FTransform& OutTransform) {}
public:

	virtual FVector GetFireSocketPos() { return FVector(); }
	virtual void Equip(FName SocketName) {}
	virtual void UnEquip() {}
	
	virtual void LMBTriggered() {}
};
