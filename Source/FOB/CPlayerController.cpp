// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerController.h"
#include "CWidgetHUD.h"

ACPlayerController::ACPlayerController() : Super()
{
	const ConstructorHelpers::FClassFinder<UCWidgetHUD> HUDFinder(TEXT("/Game/Resources/UI/BP_HUD"));

	if (HUDFinder.Class != NULL)
	{
		HUDClass = HUDFinder.Class;
	}
}

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateHUD();
}

void ACPlayerController::CreateHUD_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("CreateHUD_Implementation"));
	if (HUDClass)
	{
		PlayerHUD = CreateWidget<UCWidgetHUD>(this, HUDClass);

		if (IsValid(PlayerHUD))
		{
			UE_LOG(LogTemp, Log, TEXT("AddToViewport"));
			PlayerHUD->AddToViewport();
			PlayerHUD->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void ACPlayerController::SetDealingMonsterCharacter(ACMonsterCharacter* DealingMonsterCharacter)
{
	if (PlayerHUD == nullptr) return;
	UE_LOG(LogTemp, Log, TEXT("SetDealingMonsterCharacter"));
	PlayerHUD->SetDealingMonsterCharacter(DealingMonsterCharacter);
}
