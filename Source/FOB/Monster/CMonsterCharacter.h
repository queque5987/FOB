#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.h"
#include "CMonsterCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FDoAttack, const FString&);

UCLASS()
class FOB_API ACMonsterCharacter : public ACharacter, public IMonsterCharacter
{
	GENERATED_BODY()

public:
	ACMonsterCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextRenderComponent;

	UPROPERTY(Replicated, Transient, ReplicatedUsing = OnRep_HP)
	float fHP;
	float MaxHP;
	UFUNCTION(Server, Unreliable)
	void UpdateMonsterHP();

	UFUNCTION(Server, Unreliable)
	void DamageMonster(float DamageAmount);

	TMap<FString, class UAnimSequence*> HostileAnimMap;
	TArray<FAssetData> AssetData;

	UPROPERTY(replicated)
	class UAnimSequence* BiteAnimSequence;

	UFUNCTION(NetMulticast, Reliable)
	void SetBindDelegates();

	UFUNCTION()
	void OnRep_HP();
public:	
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UFUNCTION(Client, Unreliable)
	//virtual void SetHP(float NewHP) override;

	virtual float GetHP() override { return fHP; };
	virtual float GetMaxHP() override { return MaxHP; }

	FDoAttack DoAttack;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDoAttack(const FString& AttackType);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDoAttack(const FString& AttackType);

	virtual FVector GetBoneLocation(FName BoneName) override;

	UFUNCTION()
	void Died();
};
