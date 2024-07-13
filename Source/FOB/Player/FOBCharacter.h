// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Player/PlayerCharacter.h"
#include "FOBCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DELEGATE_OneParam(FLookAxisUpdated, FRotator);

UCLASS(config=Game)
class AFOBCharacter : public ACharacter, public IPlayerCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	class UCPlayerAnimBP* C_AnimInstance;

// Input Settings
// Input Properties

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShiftAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LMBAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RMBAction;

	float CameraBoomLength_TPS;
	float CameraBoomLength_FPS;
	float CameraRelativeLocationY_TPS;
	float CameraRelativeLocationY_FPS;
public:
	UPROPERTY(Replicated, Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_ViewRotation_Delta)
	FRotator ViewRotation_Delta;

	UPROPERTY(Replicated, Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_ViewRotation_Delta)
	FRotator ViewRotation_Delta_Zeroing;

private:
// Input Events
	UFUNCTION(Server, Reliable)
	void LMBTriggered();
	UFUNCTION(Server, Unreliable)
	void RMBStarted();
	UFUNCTION(Server, Unreliable)
	void RMBCompleted();
	UFUNCTION(Server, Unreliable)
	void ShiftTriggered();
	UFUNCTION(Server, Unreliable)
	void ShiftCompleted();

	UFUNCTION(Client, Reliable)
	void SetAimingMode(bool e, float DeltaSeconds);

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerSetViewRotation_Delta(FRotator NewViewRotation_Delta);

	UFUNCTION(Client, Reliable)
	void ClientSendViewRotation_Delta();

	UFUNCTION(Server, Reliable)
	void ServerSetViewRotation_Delta_Zeroing(FRotator NewViewRotation_Delta);

	UFUNCTION()
	void OnRep_ViewRotation_Delta();
public:
	
	FRotator GetViewRotation_Delta() { return ViewRotation_Delta; };
// Input Settings End


public:
	AFOBCharacter();

	FLookAxisUpdated LookAxisUpdated;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
			
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION()
	void OnRep_MaxWalkSpeed();

// PlayerCharacter Interface		 //
// * EquipWeapon Status				//
public:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_EquippedWeapon_R)
	AActor* EquippedWeapon_R;
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_EquippedWeapon_L)
	AActor* EquippedWeapon_L;

	UFUNCTION(Server, Reliable)
	virtual void ServerEquipItem(class AActor* WeaponToEquip) override;
	UFUNCTION(Server, Reliable)
	virtual void ServerUnEquipItem() override;

private:
	UFUNCTION()
	void OnRep_EquippedWeapon_R();
	UFUNCTION()
	void OnRep_EquippedWeapon_L();

// * EquipWeapon Status End			//

// Weapon Aiming

private:
	void AdjustNozzleToAimSpot(float DeltaSeconds);


private:
	class ACPlayerState* C_PlayerState;

	UPROPERTY(Replicated, Transient, ReplicatedUsing = OnRep_MaxWalkSpeed)
	float MinWalkSpeed;
	UPROPERTY(Replicated, Transient, ReplicatedUsing = OnRep_MaxWalkSpeed)
	float MaxWalkSpeed;
	UPROPERTY(Replicated, Transient)
	float MaxSprintSpeed;

	UFUNCTION(Server, Reliable)
	void ServerSetMaxWalkSpeed(float NewMaxWalkSpeed);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	class UTextRenderComponent* TextRenderComponent;
	UPROPERTY(Replicated)
	float fHP;

	UFUNCTION(Server, Reliable)
	void UpdateHP();
};

