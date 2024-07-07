#include "MonsterMovementComponent.h"

UMonsterMovementComponent::UMonsterMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUseAccelerationForPaths = true;
	bRequestedMoveUseAcceleration = true;
	bUseFixedBrakingDistanceForPaths = true;

	MaxAcceleration = 120.f;
	FixedPathBrakingDistance = 500.f;
}
