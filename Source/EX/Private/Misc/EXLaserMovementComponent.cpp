// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXLaserMovementComponent.h"
#include "EX.h"

void UEXLaserMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	const FVector CurrentLocation = GetOwner()->GetActorLocation();
	if (FVector::DistSquared2D(CurrentLocation, TargetLocation) > 0.001f)
	{
		const float DeltaDistance = FMath::Min(FVector::Dist(CurrentLocation, TargetLocation), Speed * DeltaTime);
		FVector DeltaLocation = (TargetLocation - CurrentLocation);
		DeltaLocation.Normalize();
		DeltaLocation *= DeltaDistance;
		const FVector NewLocation = CurrentLocation + DeltaLocation;
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult
												 , FVector(NewLocation.X, NewLocation.Y, +10000.f)
												 , FVector(NewLocation.X, NewLocation.Y, -10000.f)
												 , ECC_WorldOnly))
		{
			DeltaLocation.Z = HitResult.ImpactPoint.Z - CurrentLocation.Z;
		}
		MoveUpdatedComponent(DeltaLocation, FRotator::ZeroRotator, false, nullptr, ETeleportType::None);
	}
}
