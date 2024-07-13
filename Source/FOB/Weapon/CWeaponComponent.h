// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "CWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOB_API UCWeaponComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UCWeaponComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* Collider;

	class AActor* PossessingPlayer;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//UFUNCTION(Server, Reliable)
	//void Fire(FTransform FireTransform, class UStaticMesh* BulletMesh, float Damage);
	UFUNCTION(Server, Reliable)
	void Fire(const FVector FireLocation, class UStaticMesh* BulletMesh, float Damage);
};
