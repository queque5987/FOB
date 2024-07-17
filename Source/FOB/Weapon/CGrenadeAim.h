// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGrenadeAim.generated.h"

/**
 * 
 */
UCLASS()
class FOB_API ACGrenadeAim : public AActor
{
	GENERATED_BODY()
	
	ACGrenadeAim();
	UPROPERTY(EditAnywhere)
	class USceneComponent* Scene;
	UPROPERTY(EditAnywhere)
	TArray<class USplineMeshComponent*> SplinePoints;
	UPROPERTY(EditAnywhere)
	class UStaticMesh* SplineMesh;
public:
	void ClearSplinePoint();
	UFUNCTION(Client, Reliable)
	void AddSplinePoint(FVector StartLocation, FVector StartTangent, FVector EndLocation, FVector EndTangent);
};
