// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXAirstrikeMarker.h"
#include "Misc/EXRocket.h"
#include "EX.h"
#include "Inventory/EXHomingMovementComponent.h"

AEXAirstrikeMarker::AEXAirstrikeMarker()
{
	bCanBePickedUp = false;
}

void AEXAirstrikeMarker::BeginPlay()
{
	Super::BeginPlay();
}

void AEXAirstrikeMarker::Explode()
{
	const bool bIndoors = CheckIndoors();
	if (bIndoors)
	{
		Inventory->RecoverAbility(1.f);
	}
	else
	{
		CallAirStrike();
	}
	Super::Explode();
}

bool AEXAirstrikeMarker::CheckIndoors() const
{
	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.AddIgnoredActor(this);
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = StartLocation + FVector::UpVector * IndoorCheckDistance;

	FHitResult HitResult;
	const bool bRoof = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility_Simple, CQP);
	return bRoof;
}

void AEXAirstrikeMarker::CallAirStrike()
{
	check(RocketClass && "Rocket class is not set.");

	for (int32 Rocket = 0; Rocket < NumRockets; Rocket++)
	{
		FVector SpawnLocation = GetActorLocation() + FVector::UpVector * SpawnZ
			+ GetActorRightVector() * DistBetweenRockets * (Rocket - (NumRockets - 1))
			// Temporary, until we have the plane
			+ FVector::UpVector * 200 * Rocket;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetInstigator();
		AEXRocket* SpawnedRocket = GetWorld()->SpawnActor<AEXRocket>(RocketClass, SpawnLocation, FVector::DownVector.Rotation(), SpawnParams);
		SpawnedRocket->GetMovementComp()->Velocity = FVector::DownVector * InitialProjectileVelocity;
	}
}

