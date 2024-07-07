// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBullet.generated.h"

UCLASS()
class FOB_API ACBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBullet();

protected:

	TSubclassOf<class UDamageType> DamageType;
	class UStaticMeshComponent* StaticMeshComponent;
	class USphereComponent* Collider;

	virtual void BeginPlay() override;


	float fClock;
	float Acc;

	void CheckOverlapingActor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
