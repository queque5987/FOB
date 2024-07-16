#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/Weapon.h"
#include "CLaserRifle.generated.h"

UCLASS()
class FOB_API ACLaserRifle : public AActor, public IWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UCWeaponComponent* WeaponComponent;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;
	//UPROPERTY(EditAnywhere)
	//class UStaticMeshComponent* StaticMeshComponent_Magazine;

	//UPROPERTY(EditAnywhere)
	//class UStaticMesh* BulletStaticMesh;
public:	
	ACLaserRifle();

protected:
	virtual void BeginPlay() override;

public:	
	virtual FVector GetFireSocketPos() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void LMBTriggered() override;

	virtual FText GetWeaponName() override { return FText::FromString(TEXT("LaserRifle")); }
	//virtual UTexture2D* GetWeaponIcon() override { return nullptr; }

};
