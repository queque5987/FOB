// Fill out your copyright notice in the Description page of Project Settings.


#include "CWidgetHUD.h"

void UCWidgetHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MonsterHPBar == nullptr) return;
	if (MonsterCharacter != nullptr)
	{
		MonsterHPBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		MonsterHPBar->SetPercent(MonsterCharacter->GetHP() / MonsterCharacter->GetMaxHP());
		UE_LOG(LogTemp, Log, TEXT("HP : %f / MaxHP : %f"), MonsterCharacter->GetHP(), MonsterCharacter->GetMaxHP());
	}
	else
	{
		MonsterHPBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCWidgetHUD::SetDealingMonsterCharacter_Implementation(ACMonsterCharacter* DealingMonsterCharacter)
{
	MonsterCharacter = DealingMonsterCharacter;
}
