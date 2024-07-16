// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CFloatingUIActor.generated.h"

UCLASS()
class FOB_API ACFloatingUIActor : public AActor
{
	GENERATED_BODY()

	TSubclassOf<UUserWidget> FloatingUIClass;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* FloatingUIComponent;

public:	
	ACFloatingUIActor();

protected:
	virtual void BeginPlay() override;
	int32 fSpineIdx;
public:	
	virtual void Tick(float DeltaTime) override;

	void SetWeapon(AActor* Weapon, int32 idx);
	int32 GetCurrentTimePosAtSpine() { return fSpineIdx; };
	void SetCurrentTimePosAtSpine(int32 SpineIdx) { fSpineIdx = SpineIdx; };
	void SetOpacity(float OpacityDelta);
	void SetVisibility(bool e);
};
