// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "CWeaponFloatingUI.generated.h"

UCLASS()
class FOB_API UCWeaponFloatingUI : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponName;

	UCWeaponFloatingUI(const FObjectInitializer& ObjectInitializer);
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UObject* WeaponPtr;
	
public:
	void SetWeaponUI(AActor* Weapon, int32 idx);
};
