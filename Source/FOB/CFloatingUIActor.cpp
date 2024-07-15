#include "CFloatingUIActor.h"
#include "Components/WidgetComponent.h"
#include "CWeaponFloatingUI.h"

ACFloatingUIActor::ACFloatingUIActor()
{
	PrimaryActorTick.bCanEverTick = true;

	FloatingUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	SetRootComponent(FloatingUIComponent);

	ConstructorHelpers::FClassFinder<UCWeaponFloatingUI> FloatingUIFinder(TEXT("/Game/Resources/UI/BP_Weapon_FloatingUI"));
	if (FloatingUIFinder.Succeeded()) FloatingUIClass = FloatingUIFinder.Class;

	FloatingUIComponent->SetWidgetClass(FloatingUIClass);
	FloatingUIComponent->SetDrawSize(FVector2D(100.f, 24.f));

	FloatingUIComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
}

void ACFloatingUIActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACFloatingUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACFloatingUIActor::SetWeapon(AActor* Weapon, int32 idx)
{
	UCWeaponFloatingUI* C_WIdget = Cast<UCWeaponFloatingUI>(FloatingUIComponent->GetWidget());
	if (C_WIdget == nullptr) return;
	C_WIdget->SetWeaponUI(Weapon, idx);
}

void ACFloatingUIActor::SetOpacity(float OpacityDelta)
{
	FLinearColor LinearColor(1.f, 1.f, 1.f, OpacityDelta);
	FloatingUIComponent->SetTintColorAndOpacity(LinearColor);
}

