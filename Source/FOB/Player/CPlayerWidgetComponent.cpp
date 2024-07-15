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

	int32 SelectedWeapon = FloatingUIActorArr.Num() * 2;
	for (int i = 0; i < FloatingUIActorArr.Num(); i++)
	{
		FVector LocationAtTime = SplineComponent->GetLocationAtTime(UITimes[i], ESplineCoordinateSpace::World);
		FloatingUIActorArr[i]->SetActorLocation(LocationAtTime);
		FloatingUIActorArr[i]->SetActorRotation(GetOwner()->GetActorRotation() + FRotator(0.f, 180.f, 0.f));
		float Opacity = 1.f - FMath::Abs(FMath::Atan(UITimes[i] - 0.5f) * 5.f);
		if (UITimes[i] > 0.45f - DeltaTime && UITimes[i] < 0.55f + DeltaTime)
		{
			SelectedWeapon = i;
			if (SelectedWeapon == FMath::Abs(SelectedUI)) Opacity = 1.f;
		}
		FloatingUIActorArr[i]->SetOpacity(Opacity);
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
	UE_LOG(LogTemp, Log, TEXT("UCPlayerWidgetComponent Tick : %s"), *out);
	//UITime1 += RotateCounterClockWise ? DeltaTime : -DeltaTime;
	//UITime2 += RotateCounterClockWise ? DeltaTime : -DeltaTime;
	//UITime3 += RotateCounterClockWise ? DeltaTime : -DeltaTime;

	//if (UITime1 > 1.f) UITime1 -= 1.f;
	//else if (UITime1 < 0.f) UITime1 += 1.f;
	//if (UITime2 > 1.f) UITime2 -= 1.f;
	//else if (UITime2 < 0.f) UITime2 += 1.f;
	//if (UITime3 > 1.f) UITime3 -= 1.f;
	//else if (UITime3 < 0.f) UITime3 += 1.f;
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
		UITimes[i] = 1.f / (FloatingUIActorArr.Num() + 1.f) * (FloatingUIActorArr.Num() - (i + 2));
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

