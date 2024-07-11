// Fill out your copyright notice in the Description page of Project Settings.


#include "CMonsterAnimBP.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Monster/CMonsterCharacter.h"

UCMonsterAnimBP::UCMonsterAnimBP() : Super()
{

}

void UCMonsterAnimBP::NativeInitializeAnimation()
{ 
	Super::NativeInitializeAnimation();

	Pawn = TryGetPawnOwner();
	PawnVelocity = 0.f;

	if (Pawn == nullptr) return;
	ACMonsterCharacter* MonsterCharacter = Cast<ACMonsterCharacter>(Pawn);
	if (MonsterCharacter == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("AIController Cannot Found"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("AIController Binding Delegates"));
	
	//MonsterCharacter->DoAttack.BindUFunction(this, FName("DoAttack"));
}

void UCMonsterAnimBP::UpdateProperties(float Delta)
{
	if (Pawn == nullptr) return;

	PawnVelocity = Pawn->GetVelocity().Size();
	PawnMaxVelocity = Pawn->GetMovementComponent()->GetMaxSpeed();
}

//void UCMonsterAnimBP::DoAttack(const FString AttackType)
//{
//	UAnimSequence** tempAnimSequence = HostileAnimMap.Find(AttackType);
//
//	if (tempAnimSequence == nullptr)
//	{
//		UE_LOG(LogTemp, Log, TEXT("UCMonsterAnimBP : AttackType %s Can Not Found "), *AttackType);
//		return;
//	}
//	ClientDoAttack(*tempAnimSequence);
//}
//
//void UCMonsterAnimBP::ClientDoAttack_Implementation(UAnimSequence* ToPlayAnimSequence)
//{
//	PlaySlotAnimationAsDynamicMontage(ToPlayAnimSequence, "DefaultSlot", 0.25f, 0.25f, 1.f);
//}

