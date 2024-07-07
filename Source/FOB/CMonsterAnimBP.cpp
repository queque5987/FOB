// Fill out your copyright notice in the Description page of Project Settings.


#include "CMonsterAnimBP.h"
#include "GameFramework/PawnMovementComponent.h"

UCMonsterAnimBP::UCMonsterAnimBP() : Super()
{
}

void UCMonsterAnimBP::NativeInitializeAnimation()
{ 
	Super::NativeInitializeAnimation();

	Pawn = TryGetPawnOwner();
	PawnVelocity = 0.f;
}

void UCMonsterAnimBP::UpdateProperties(float Delta)
{
	if (Pawn == nullptr) return;

	PawnVelocity = Pawn->GetVelocity().Size();
	PawnMaxVelocity = Pawn->GetMovementComponent()->GetMaxSpeed();
}
