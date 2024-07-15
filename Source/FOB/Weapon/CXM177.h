#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/Weapon.h"
#include "CXM177.generated.h"

UCLASS()
class FOB_API ACXM177 : public AActor, public IWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class UCWeaponComponent* WeaponComponent;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComponent_Magazine;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* BulletStaticMesh;

public:	
	ACXM177();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	virtual FVector GetFireSocketPos() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void LMBTriggered() override;

	virtual FText GetWeaponName() override { return FText::FromString(TEXT("XM - 177")); }
	//virtual UTexture2D* GetWeaponIcon() override { return nullptr; }
};
