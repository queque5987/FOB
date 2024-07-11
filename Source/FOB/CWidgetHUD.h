// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Monster/CMonsterCharacter.h"
#include "CWidgetHUD.generated.h"

UCLASS()
class FOB_API UCWidgetHUD : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> MonsterHPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> PlayerProfile;
	
	ACMonsterCharacter* MonsterCharacter;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(Client, Unreliable)
	void SetDealingMonsterCharacter(ACMonsterCharacter* DealingMonsterCharacter);
	void SetDealingMonsterCharacter_Implementation(ACMonsterCharacter* DealingMonsterCharacter);
};
