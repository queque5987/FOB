// Fill out your copyright notice in the Description page of Project Settings.


#include "CWeaponFloatingUI.h"
#include "Weapon/Weapon.h"

UCWeaponFloatingUI::UCWeaponFloatingUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponPtr = nullptr;
}

void UCWeaponFloatingUI::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IWeapon* I_Weapon = Cast<IWeapon>(ListItemObject);
	if (I_Weapon == nullptr) return;
	WeaponPtr = ListItemObject;

	WeaponName->SetText(I_Weapon->GetWeaponName());
}

void UCWeaponFloatingUI::SetWeaponUI(AActor* Weapon, int32 idx)
{
	IWeapon* I_Weapon = Cast<IWeapon>(Weapon);
	if (I_Weapon == nullptr) return;
	//WeaponPtr = ListItemObject;
	
	WeaponName->SetText(FText::FromString(TEXT("[") + FString::FromInt(idx) + TEXT("] ") + I_Weapon->GetWeaponName().ToString()));
}
