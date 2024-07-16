// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerWidgetComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SplineComponent.h"
#include "CWeaponFloatingUI.h"
#include "CFloatingUIActor.h"

UCPlayerWidgetComponent::UCPlayerWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(this);
	SplineComponent->ClearSplinePoints();
	SplineComponent->AddSplinePoint(FVector(-50.f, -50.f, 100.f), ESplineCoordinateSpace::Local);
	SplineComponent->AddSplinePoint(FVector(-100.f, 0.f, 100.f), ESplineCoordinateSpace::Local);
	SplineComponent->AddSplinePoint(FVector(-50.f, 50.f, 100.f), ESplineCoordinateSpace::Local);

	SplineComponent->SetTangentAtSplinePoint(0, FVector(-50.f, 0.f, 0.f), ESplineCoordinateSpace::Local);
	SplineComponent->SetTangentAtSplinePoint(1, FVector(0.f, 100.f, 0.f), ESplineCoordinateSpace::Local);
	SplineComponent->SetTangentAtSplinePoint(2, FVector(50.f, 0.f, 0.f), ESplineCoordinateSpace::Local);

	SplineComponent->SetRelativeLocation(FVector(-30.f, 0.f, 0.f));
	SplineComponent->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
}

void UCPlayerWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (VisibleCounter <= 0.f) SetFloatingActorstVisibility(false);
	else VisibleCounter -= DeltaTime;
	
	int32 SelectedWeapon = FloatingUIActorArr.Num() * 2;
	for (int i = 0; i < FloatingUIActorArr.Num(); i++)
	{
		FVector LocationAtTime = SplineComponent->GetLocationAtTime(UITimes[i], ESplineCoordinateSpace::World);
		FloatingUIActorArr[i]->SetActorLocation(LocationAtTime);
		FloatingUIActorArr[i]->SetActorRotation(GetOwner()->GetActorRotation() + FRotator(0.f, 180.f, 0.f));
		//float Opacity = 1.f - FMath::Abs(FMath::Atan(UITimes[i] - 0.5f) * 1.2f);
		//float Opacity = FMath::Min((FMath::Atan(VisibleCounter / 3.f) * 1.2f), 1.f);
		FVector Scale(0.3f, 0.3f, 0.3f);
		if (UITimes[i] > 0.45f - DeltaTime && UITimes[i] < 0.55f + DeltaTime)
		{
			SelectedWeapon = i;
			if (SelectedWeapon == FMath::Abs(SelectedUI))
			{
				//Opacity = 1.f;
				Scale *= 1.3f;
			}
		}
		//FloatingUIActorArr[i]->SetOpacity(Opacity);
		FloatingUIActorArr[i]->SetActorRelativeScale3D(Scale * 
			(1 - FMath::Abs(FMath::Atan(UITimes[i] - 0.5f))) * // Scale Per Location At Spline
			FMath::Min( // No Minus
				(
					FMath::Atan( // Closer VisibleCounter To 0 = Faster Decline 
						VisibleCounter * 30.f
					)
				), 1.f
			)
		);
	}
	
	if (SelectedWeapon == FMath::Abs(SelectedUI)) return;

	FString out;

	for (int i = 0; i < UITimes.Num(); i++)
	{
		UITimes[i] += RotateCounterClockWise ? DeltaTime : -DeltaTime;
		if (UITimes[i] > 1.f) UITimes[i] -= 1.f;
		else if (UITimes[i] < 0.f) UITimes[i] += 1.f;

		out += FString::FromInt(i);
		out += TEXT(" = ");
		out += FString::SanitizeFloat(UITimes[i]);
	}
}

void UCPlayerWidgetComponent::SetFloatingActorstVisibility(bool e)
{
	for (ACFloatingUIActor* FloatingUIActor : FloatingUIActorArr)
	{
		FloatingUIActor->SetVisibility(e);
	}
}

void UCPlayerWidgetComponent::ClearFloatingUI()
{
	// Clear Existed Arrs
	FloatingUIActorArr.Empty();
	UITimes.Empty();
}

void UCPlayerWidgetComponent::AddFloatingUI(AActor* NewWeapon)
{
	ACFloatingUIActor* A_UI = GetWorld()->SpawnActor<ACFloatingUIActor>(ACFloatingUIActor::StaticClass(), GetComponentLocation(), GetComponentRotation(), FActorSpawnParameters());
	A_UI->SetWeapon(NewWeapon, FloatingUIActorArr.Num());
	A_UI->SetCurrentTimePosAtSpine(FloatingUIActorArr.Num());
	FloatingUIActorArr.Add(A_UI);
	UITimes.Add(0.f);
	for (int i = 0; i < UITimes.Num(); i++)
	{
		UITimes[i] = 1.f / (FloatingUIActorArr.Num() + ((FloatingUIActorArr.Num() % 2 == 0) ? 1.f : 2.f)) * (FloatingUIActorArr.Num() - (i + 2));
	}
}

void UCPlayerWidgetComponent::AddFloatingUIArr(int32 FloatingUIActorIndex)
{
	if (FloatingUIActorArr.Num() == 0) return;
	RotateCounterClockWise = FloatingUIActorIndex < 0 ? false : true;
	//UE_LOG(LogTemp, Log, TEXT("UCPlayerWidgetComponent : (%d + %d) % %d"), SelectedUI, FloatingUIActorIndex, FloatingUIActorArr.Num());
	SelectedUI = (SelectedUI + FloatingUIActorIndex) % FloatingUIActorArr.Num();
	if (SelectedUI < 0) SelectedUI = FloatingUIActorArr.Num() - 1;
	UE_LOG(LogTemp, Log, TEXT("UCPlayerWidgetComponent : AddFloatingUIArr - %d"), SelectedUI);
}

void UCPlayerWidgetComponent::SetTemporaryVisible()
{
	VisibleCounter = ResetVisibleCounter;
	SetFloatingActorstVisibility(true);
}

void UCPlayerWidgetComponent::RefreshFloatingUI(TArray<AActor*> PossessingWeapons)
{
// Clear Existed Arrs
	FloatingUIActorArr.Empty();
	UITimes.Empty();
	
// Add UI For Every Weapon
	for (AActor* WeaponInPocket : PossessingWeapons)
	{
		//AddFloatingUI(WeaponInPocket);
		UE_LOG(LogTemp, Log, TEXT("UCPlayerWidgetComponent : RefreshFloatingUI - %s"), *WeaponInPocket->GetName());

		ACFloatingUIActor* A_UI = GetWorld()->SpawnActor<ACFloatingUIActor>(ACFloatingUIActor::StaticClass(), GetComponentLocation(), GetComponentRotation(), FActorSpawnParameters());
		A_UI->SetWeapon(WeaponInPocket, FloatingUIActorArr.Num());
		A_UI->SetCurrentTimePosAtSpine(FloatingUIActorArr.Num());
		FloatingUIActorArr.Add(A_UI);
		UITimes.Add(0.f);
	}

// Arrange UITimes (Location At Spline)
	for (int i = 0; i < UITimes.Num(); i++)
	{
		UITimes[i] = 1.f / (FloatingUIActorArr.Num() + ((FloatingUIActorArr.Num() % 2 == 0) ? 1.f : 2.f)) * (FloatingUIActorArr.Num() - (i + 2));
	}
}

