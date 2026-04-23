// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXRocketLauncher.h"
#include "Misc/EXRocket.h"
#include "Player/EXCharacter.h"
#include "Components/SceneComponent.h"
#include "EX.h"
#include "Inventory/EXTargettingComponent.h"
#include "Inventory/EXHomingMovementComponent.h"

UEXRocketLauncher::UEXRocketLauncher()
{
	/*PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	PrimaryComponentTick.TickInterval = .1f;*/
	//SetIsReplicatedByDefault(true);
}

void UEXRocketLauncher::PerformShot()
{
	if (!IsLocallyOwned())
	{
		return;
	}
	const FRotator Rot = GetOwningCharacter()->GetControlRotation();
	const FVector Loc = GetWeaponMeshComponent()->GetComponentLocation() + Rot.RotateVector(ProjectileSpawnOffset);
	Server_Reliable_PerformShot(Loc, Rot);
}

void UEXRocketLauncher::BeginPlay()
{
	Super::BeginPlay();

	/*if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		PrimaryComponentTick.SetTickFunctionEnable(true);
	}*/
}

bool UEXRocketLauncher::Tick(float DeltaTime)
{
	bool bAnythingChanged = Super::Tick(DeltaTime);
	if (GetOwnerRole() != ROLE_AutonomousProxy)
	{
		return bAnythingChanged;
	}
	// This tick should only be running on autonomous proxies
	if (bHoming)
	{
		FVector PlayerEyesLoc;
		FRotator PlayerEyesRot;
		Player->GetActorEyesViewPoint(PlayerEyesLoc, PlayerEyesRot);

		FCollisionQueryParams CQP;
		CQP.AddIgnoredActor(Player);
		FHitResult HitResult;
		FVector TraceEnd = PlayerEyesLoc + PlayerEyesRot.Vector() * TraceRange;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, PlayerEyesLoc, TraceEnd, ECC_Visibility_Simple, CQP))
		{
			TraceEnd = HitResult.ImpactPoint;
		}
		Server_Unreliable_SetHomingTarget(TraceEnd);
	}
	return bAnythingChanged;
}

void UEXRocketLauncher::Server_Reliable_PerformShot_Implementation(const FVector Loc, const FRotator Rot)
{
	check(RocketClass && "Rocket class not set");

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwningCharacter();
	SpawnParams.Instigator = GetOwningCharacter();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEXRocket* Rocket = GetWorld()->SpawnActor<AEXRocket>(RocketClass, Loc, Rot, SpawnParams);
	Rocket->GetMovementComp()->SetIsHoming(bHoming);
	if (bHoming)
	{
		Rocket->OnExploded.AddDynamic(this, &UEXRocketLauncher::RocketExploded);
		HomingRockets.Add(Rocket);
		if (HomingRockets.Num() == 1)
		{
			Client_Reliable_SetHomingEnabled(true);
		}
	}
}

bool UEXRocketLauncher::Server_Reliable_PerformShot_Validate(const FVector Loc, const FRotator Rot)
{
	return true;
}

void UEXRocketLauncher::Client_Reliable_SetHomingEnabled_Implementation(bool bEnabled)
{
	bHoming = bEnabled;
}

void UEXRocketLauncher::Server_Unreliable_SetHomingTarget_Implementation(const FVector& Loc)
{
	for (AEXRocket* Rocket : HomingRockets)
	{
		Rocket->SetTarget(Loc);
	}
}

bool UEXRocketLauncher::Server_Unreliable_SetHomingTarget_Validate(const FVector& Loc)
{
	return true;
}

void UEXRocketLauncher::RocketExploded(AEXRocket* Rocket)
{
	HomingRockets.Remove(Rocket);
	if (HomingRockets.Num() == 0)
	{
		Client_Reliable_SetHomingEnabled(false);
	}
}
