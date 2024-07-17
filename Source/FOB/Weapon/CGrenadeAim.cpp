// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/CGrenadeAim.h"
#include "Engine/SplineMeshActor.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"

ACGrenadeAim::ACGrenadeAim()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(Scene);
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/Megascans/3D_Assets/Potato_vfdlbdrjw/S_Potato_vfdlbdrjw_lod3_Var1"));
	if (SMFinder.Succeeded()) SplineMesh = SMFinder.Object;
}

void ACGrenadeAim::ClearSplinePoint()
{
	for (USplineMeshComponent* SplinePoint : SplinePoints)
	{
		SplinePoint->DestroyComponent();
	}
	SplinePoints.Empty();
}

void ACGrenadeAim::AddSplinePoint_Implementation(FVector StartLocation, FVector StartTangent, FVector EndLocation, FVector EndTangent)
{
	USplineMeshComponent* SplinePoint = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

	SplinePoint->SetMobility(EComponentMobility::Movable);
	SplinePoint->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
	SplinePoint->SetStaticMesh(SplineMesh);

	SplinePoint->AttachToComponent(Scene, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 40.f, FColor::Blue);

	SplinePoints.Add(SplinePoint);
}
