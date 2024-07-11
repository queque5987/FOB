// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/Weapon.h"
#include "CAr15.generated.h"

UCLASS()
class FOB_API ACAr15 : public AActor, public IWeapon
{
	GENERATED_BODY()
	
	class UCWeaponComponent* WeaponComponent;
	class UStaticMeshComponent* StaticMeshComponent;

public:	
	ACAr15();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	//virtual void Equip(FName SocketName) 
	//virtual void UnEquip()
	
};
