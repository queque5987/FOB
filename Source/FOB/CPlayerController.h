// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UIController.h"
#include "CPlayerController.generated.h"

UCLASS()
class FOB_API ACPlayerController : public APlayerController, public IUIController
{
	GENERATED_BODY()
	
	ACPlayerController();
	
	virtual void BeginPlay() override;

	TSubclassOf<class UCWidgetHUD> HUDClass;
	class UCWidgetHUD* PlayerHUD;

private:
	UFUNCTION(Client, Unreliable)
	void CreateHUD();
	virtual void CreateHUD_Implementation();

public:
	virtual void SetDealingMonsterCharacter(class ACMonsterCharacter* DealingMonsterCharacter) override;
};
