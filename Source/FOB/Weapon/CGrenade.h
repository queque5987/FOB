// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGrenade.generated.h"

UCLASS()
class FOB_API ACGrenade : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;
public:	
	ACGrenade();

protected:
	virtual void BeginPlay() override;

	void CheckOverlapingActor();
public:	
	virtual void Tick(float DeltaTime) override;

	
	
};
