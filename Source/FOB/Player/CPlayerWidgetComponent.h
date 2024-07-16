// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "CPlayerWidgetComponent.generated.h"

UCLASS()
class FOB_API UCPlayerWidgetComponent : public USceneComponent
{
	GENERATED_BODY()
	

	UCPlayerWidgetComponent();
	
	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere)
	TArray<class ACFloatingUIActor*> FloatingUIActorArr;
	TArray<float> UITimes;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	float RotatePower;
	int32 SelectedUI = 0;
	bool RotateCounterClockWise = true;
	float VisibleCounter = 0.f;
	const float ResetVisibleCounter = 2.f;

	void SetFloatingActorstVisibility(bool e);

public:
	void ClearFloatingUI();
	void AddFloatingUI(AActor* NewWeapon);
	void AddFloatingUIArr(int32 FloatingUIActorIndex);
	void SetTemporaryVisible();
	//Deprecated
	void RefreshFloatingUI(TArray<AActor*> PossessingWeapons);
};
