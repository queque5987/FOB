#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "MonsterCharacter.h"
#include "CMonsterCharacter.generated.h"

UCLASS()
class FOB_API ACMonsterCharacter : public APawn, public IMonsterCharacter
{
	GENERATED_BODY()

public:
	ACMonsterCharacter();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComponent;
	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextRenderComponent;

	UPROPERTY(replicated)
	float fHP;
	float MaxHP;
	UFUNCTION(Server, Unreliable)
	void UpdateMonsterHP();
	void UpdateMonsterHP_Implementation();

	UFUNCTION(Server, Unreliable)
	void DamageMonster(float DamageAmount);
	void DamageMonster_Implementation(float DamageAmount);

public:	
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Client, Unreliable)
	virtual void SetHP(float NewHP) override;
	virtual void SetHP_Implementation(float NewHP);

	virtual float GetHP() override { return fHP; };
	virtual float GetMaxHP() override { return MaxHP; }
};
