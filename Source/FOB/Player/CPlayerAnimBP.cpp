// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerAnimBP.h"
#include "Player/FOBCharacter.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/SpringArmComponent.h"

UCPlayerAnimBP::UCPlayerAnimBP() : Super()
{

}

void UCPlayerAnimBP::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPlayer = Cast<AFOBCharacter>(TryGetPawnOwner());

	if (OwningPlayer == nullptr) return;

	OwningPlayer->LookAxisUpdated.BindUFunction(this, FName("SetViewRotation_Delta"));
}

void UCPlayerAnimBP::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCPlayerAnimBP, MovementVector);
	DOREPLIFETIME(UCPlayerAnimBP, ViewRotation_Delta);
}

void UCPlayerAnimBP::UpdateProperties(float DeltaSeconds)
{
	if (OwningPlayer == nullptr) return;

	// Movement Param
	FRotator ViewRotation = OwningPlayer->GetViewRotation();
	FRotator ViewRotatorYaw = FRotator(0.f, ViewRotation.Yaw, 0.f);
	FVector RelativeVelocityNormalizedVector = ViewRotatorYaw.UnrotateVector(OwningPlayer->GetVelocity());
	MovementVector.Set(RelativeVelocityNormalizedVector.X, RelativeVelocityNormalizedVector.Y);

	// AimOffset Param
	//ViewRotation_Delta = (ViewRotation - OwningPlayer->GetActorRotation()).GetNormalized();
}

void UCPlayerAnimBP::SetViewRotation_Delta_Implementation(FRotator NewViewRotation_Delta)
{
	ViewRotation_Delta = NewViewRotation_Delta;
}
