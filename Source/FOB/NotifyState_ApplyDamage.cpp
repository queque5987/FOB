#include "NotifyState_ApplyDamage.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "FOBCharacter.h"
#include "MonsterCharacter.h"
#include "PCH.h"

void UNotifyState_ApplyDamage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	//UE_LOG(LogTemp, Log, TEXT("NotifyBegin"));
}

void UNotifyState_ApplyDamage::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp->GetOwner()->HasAuthority()) return;

	FHitResult HitResult;
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams tempParam;
	FCollisionShape tempCollisionShpae;

	tempParam.AddObjectTypesToQuery(ECC_Pawn);
	tempCollisionShpae.SetSphere(120.f);

	FVector HeadPos = MeshComp->GetBoneLocation("MOUNTAIN_DRAGON_-Head");

	bool bHit = MeshComp->GetWorld()->SweepMultiByObjectType(
		HitResults,
		HeadPos,
		HeadPos,
		FQuat::Identity,
		tempParam,
		tempCollisionShpae
	);

	DrawDebugSphere(MeshComp->GetWorld(), HeadPos, 120.f, 32.f, bHit ? FColor::Green : FColor::Red);

	//if (!bHit) UE_LOG(LogTemp, Log, TEXT("No Hit Actor"));

	if (bHit)
	{
		for (const FHitResult HR : HitResults)
		{
			//UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HR.GetActor()->GetName());

			AFOBCharacter* Player = Cast<AFOBCharacter>(HR.GetActor());
			if (Player != nullptr)
			{
				FDamageEvent DamageEvent = FDamageEvent();

				UE_LOG(LogTemp, Log, TEXT("Monster Dealt Damage to Player"));

				UGameplayStatics::ApplyPointDamage(Player, 1.f, Player->GetActorLocation() - HR.Location, HR, nullptr, MeshComp->GetOwner(), NULL);
			}
		}
	}
}

void UNotifyState_ApplyDamage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	//UE_LOG(LogTemp, Log, TEXT("NotifyEnd"));
}
