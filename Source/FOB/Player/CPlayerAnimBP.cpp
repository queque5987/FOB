// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerAnimBP.h"
#include "Player/FOBCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/CPlayerState.h"
#include "PCH.h"

UCPlayerAnimBP::UCPlayerAnimBP() : Super()
{
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

//void UCPlayerAnimBP::OnPlayerAnimStatusUpdated(int32 PlayerAnimStatus)
//{
//	bCrouching = PLAYER_CROUCH & PlayerAnimStatus;
//	UE_LOG(LogTemp, Log, TEXT("UCPlayerAnimBP - OnPlayerAnimStatusUpdated %s"), bCrouching ? TEXT("True"): TEXT("False"));
//}

void UCPlayerAnimBP::SetupDelegates_Implementation()
{
	if (OwningPlayer == nullptr) return;
	OwningPlayer->PlayerAnimStatusUpdated.BindUFunction(this, TEXT("SetbCrouching"));

	//C_PlayerState = Cast<ACPlayerState>(OwningPlayer->GetPlayerState());
	//if (C_PlayerState == nullptr) return;
	//C_PlayerState->PlayerAnimStatusUpdated.BindUFunction(this, TEXT("OnPlayerAnimStatusUpdated"));

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
	//if (C_PlayerState != nullptr)
	//{
		//ServerSetbCrouching(C_PlayerState->GetPlayerAnimStatus(PLAYER_CROUCH));
		//bCrouching = C_PlayerState->GetPlayerAnimStatus(PLAYER_CROUCH);
		//UE_LOG(LogTemp, Log, TEXT("UCPlayerAnimBP - UpdateProperties %s"), bCrouching ? TEXT("True") : TEXT("False"));
	//}
	bCrouching = OwningPlayer->GetbCrouching();
	// Movement Param
	FRotator ViewRotation = OwningPlayer->GetViewRotation();
	FRotator ViewRotatorYaw = FRotator(0.f, ViewRotation.Yaw, 0.f);
	FVector RelativeVelocityNormalizedVector = ViewRotatorYaw.UnrotateVector(OwningPlayer->GetVelocity());

	MovementVector.Set(RelativeVelocityNormalizedVector.X, RelativeVelocityNormalizedVector.Y);

	ViewRotation_Delta = OwningPlayer->GetViewRotation_Delta();
}

void UCPlayerAnimBP::SetViewRotation_Delta(FRotator NewViewRotation_Delta)
{
	ViewRotation_Delta = NewViewRotation_Delta;
}
