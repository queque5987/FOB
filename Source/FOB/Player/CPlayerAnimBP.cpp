// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerAnimBP.h"
#include "Player/FOBCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/CPlayerState.h"
#include "PCH.h"

UCPlayerAnimBP::UCPlayerAnimBP() : Super()
{
	ConstructorHelpers::FObjectFinder<UAnimSequence> RiflePutAwayFinder(TEXT("/Game/Resources/Character/PlayerAsset/Animation/AnimSeqeunce/Equip/Rifle_Put_Away"));
	ConstructorHelpers::FObjectFinder<UAnimSequence> RiflePullOutFinder(TEXT("/Game/Resources/Character/PlayerAsset/Animation/AnimSeqeunce/Equip/Rifle_Pull_Out"));

	if (RiflePutAwayFinder.Succeeded()) RiflePutAway = RiflePutAwayFinder.Object;
	if (RiflePullOutFinder.Succeeded()) RiflePullOut = RiflePullOutFinder.Object;
}

void UCPlayerAnimBP::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPlayer = Cast<AFOBCharacter>(TryGetPawnOwner());
	if (OwningPlayer == nullptr) return;
}

void UCPlayerAnimBP::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCPlayerAnimBP, bCrouching)
}

void UCPlayerAnimBP::SetbCrouching(bool e)
{
	bCrouching = e;
}

void UCPlayerAnimBP::SetupDelegates_Implementation()
{
	if (OwningPlayer == nullptr) return;
	OwningPlayer->PlayerAnimStatusUpdated.BindUFunction(this, TEXT("SetbCrouching"));

	UE_LOG(LogTemp, Log, TEXT("UCPlayerAnimBP - SetupDelegates"));
}

void UCPlayerAnimBP::ServerSetbCrouching_Implementation(bool e)
{
	ClientSetbCrouching(e);
	UE_LOG(LogTemp, Log, TEXT("UCPlayerAnimBP - ServerSetbCrouching %s"), bCrouching ? TEXT("True") : TEXT("False"));
}
//
void UCPlayerAnimBP::ClientSetbCrouching_Implementation(bool e)
{
	bCrouching = e;
	UE_LOG(LogTemp, Log, TEXT("UCPlayerAnimBP - ClientSetbCrouching %s"), bCrouching ? TEXT("True") : TEXT("False"));
}

void UCPlayerAnimBP::UpdateProperties(float DeltaSeconds)
{
	if (OwningPlayer == nullptr)
	{
		OwningPlayer = Cast<AFOBCharacter>(TryGetPawnOwner());
		return;
	}

	bCrouching = OwningPlayer->GetbCrouching();
	//bRightHandFull = OwningPlayer->GetEquippedWeapon_R() != nullptr ? true : false;
	bRightHandFull = OwningPlayer->IsEquippingWeapon();
	// Movement Param
	FRotator ViewRotation = OwningPlayer->GetViewRotation();
	FRotator ViewRotatorYaw = FRotator(0.f, ViewRotation.Yaw, 0.f);
	FVector RelativeVelocityNormalizedVector = ViewRotatorYaw.UnrotateVector(OwningPlayer->GetVelocity());

	MovementVector.Set(RelativeVelocityNormalizedVector.X, RelativeVelocityNormalizedVector.Y);

	ViewRotation_Delta = OwningPlayer->GetViewRotation_Delta();
}

void UCPlayerAnimBP::PlayRiflePullOut()
{
	PlaySlotAnimationAsDynamicMontage(RiflePullOut, "UpperBody", 0.25f, 0.25f, 3.f);
}

void UCPlayerAnimBP::PlayRiflePutAway()
{
	PlaySlotAnimationAsDynamicMontage(RiflePutAway, "UpperBody", 0.25f, 0.25f, 1.f);
}

void UCPlayerAnimBP::SetViewRotation_Delta(FRotator NewViewRotation_Delta)
{
	ViewRotation_Delta = NewViewRotation_Delta;
}
