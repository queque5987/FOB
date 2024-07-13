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
	ACBullet();

protected:

	TSubclassOf<class UDamageType> DamageType;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(EditAnywhere)
	class USphereComponent* Collider;

	virtual void BeginPlay() override;


	float fClock;
	float Acc;

	void CheckOverlapingActor();

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(NetMulticast, Reliable)
	void SetStaticMesh(class UStaticMesh* NewStaticMesh);
};
