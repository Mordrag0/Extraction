// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXWeaponTrace.h"
#include "GameFramework/DamageType.h"
#include "EX.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Player/EXCharacter.h"
#include "Online/EXPlayerState.h"
#include "Engine/EngineTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Inventory/EXDamageType.h"
#include "Inventory/EXWeapon.h"
#include "Inventory/EXFireMode.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// #DEBUG
static TAutoConsoleVariable<int32> CVarShowTraceDebug(
	TEXT("x.ShowTraceDebug"),
	0,
	TEXT("Enables rendering debug lines for trace weapons.\n")
	TEXT("  0: off\n")
	TEXT("  1: on\n"),
	ECVF_Cheat);
#endif

bool UEXWeaponTrace::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Actions.Contains(Mode))
	{
		// Nothing to do
		return false;
	}
	if (Mode == EWeaponInput::Primary) 
	{
		FireValidated = 1;
	}
	if (!Super::Fire(Mode, WorldTimeOverride))
	{
		if (Mode == EWeaponInput::Primary)
		{
			FireValidated = 0;
		}

		return false;
	}

	return true;
}

void UEXWeaponTrace::PerformShot()
{
	Super::PerformShot();

	PerformTrace();
}

void UEXWeaponTrace::PerformTrace(float ShotCharge)
{
	if (!IsLocallyOwned())
	{
		return;
	}

	const float WorldTime = GetWorld()->GetTimeSeconds();
	RecalculateSpread(WorldTime);

	FVector StartLocation;
	FRotator ViewDirection;
	GetOwningCharacter()->GetActorEyesViewPoint(StartLocation, ViewDirection);
	FVector TraceDirection = GetTraceDirection(ViewDirection);

	FVector EndLocation = StartLocation + (TraceDirection * MaxTraceRange);;

	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.AddIgnoredActor(GetOwningCharacter());
	CQP.bReturnPhysicalMaterial = true;
	CQP.bTraceComplex = true;

	FHitResult HitResult;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WeaponTrace, CQP) || !HitResult.GetActor())
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const int32 bShowDebug = CVarShowTraceDebug.GetValueOnGameThread();
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, false, 5.f);
		}
#endif
		PlayTraceEffects(EndLocation);
		TraceNoHit(EndLocation);
	}
	else // Hit
	{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const int32 bShowDebug = CVarShowTraceDebug.GetValueOnGameThread();
		if (bShowDebug)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Yellow, false, 5.f);
		}
#endif
		SpawnImpactEffect(HitResult);
		PlayTraceEffects(HitResult.ImpactPoint);
		TraceHit(HitResult, ShotCharge);
		if (HitResult.GetActor()->CanBeDamaged())
		{
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
			HitOnClient(HitResult.GetActor(), (SurfaceType == SURFACE_Head));
		}
	}
}

bool UEXWeaponTrace::Server_Unreliable_TraceHit_Validate(const FHitResult& HitResult, float ShotCharge)
{
	return true;
}

void UEXWeaponTrace::Server_Unreliable_TraceHit_Implementation(const FHitResult& HitResult, float ShotCharge)
{
	TraceHit(HitResult, ShotCharge);
}

void UEXWeaponTrace::Server_Unreliable_TraceNoHit_Implementation(const FVector_NetQuantize TraceEnd)
{
	TraceNoHit(TraceEnd);
}


bool UEXWeaponTrace::ValidateTrace(const FHitResult& HitResult) const
{
	if (!HitResult.GetActor())
	{
		return false;
	}

	if (FVector::Dist(HitResult.TraceStart, GetOwningCharacter()->GetActorLocation()) > MaxOriginTolerance)
	{
		return false;
	}

	//if (FVector::Dist(HitResult.ImpactPoint, HitResult.GetActor()->GetActorLocation()) > MaxTargetTolerance) #EXTODO2 need to calculate bounding box
	//{
	//	return false;
	//}

	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.AddIgnoredActor(GetOwningCharacter());
	CQP.AddIgnoredActor(HitResult.GetActor());

	// Check if any static objects are between the trace start and impact point
	FHitResult TestHit;
	if (GetWorld()->LineTraceSingleByChannel(TestHit, HitResult.TraceStart, HitResult.ImpactPoint, ECC_WorldOnly, CQP))
	{
		return false;
	}

	return true;
}

void UEXWeaponTrace::TraceHit(const FHitResult& HitResult, float ShotCharge)
{
	if (!IsAuthority())
	{
		Server_Unreliable_TraceHit(HitResult, ShotCharge);
		return;
	}
	Multicast_PlayTraceEffects(HitResult.ImpactPoint);
	Multicast_SpawnImpactEffect(HitResult);

	AEXPlayerState* PS = GetOwningCharacter()->GetPlayerState<AEXPlayerState>();
	if (!HitResult.GetActor() || !HitResult.GetActor()->CanBeDamaged())
	{
		if (PS)
		{
			PS->ShotMissed(GetOwningCharacter()->GetMercType(), GetWeaponType());
		}
		return;
	}
	/*if (FireValidated == 0) // #EXTODO2
	{
		return;
	}*/

	FireValidated--;

	if (!ValidateTrace(HitResult))
	{
		UE_LOG(LogEXWeapon, Warning, TEXT("Trace weapon (%s) failed validation: %s"), *GetName(), *HitResult.GetActor()->GetName());
		return;
	}

	// Linear damage falloff
	const float Range = FVector::Distance(HitResult.TraceStart, HitResult.ImpactPoint);
	const float RangeModifier = FMath::Clamp((Range - MinRange) / (MaxRange - MinRange), 0.f, 1.f);
	const float ActualDamage = Damage * (1 - RangeModifier) + MinRangeDamage * RangeModifier;
	UE_LOG(LogEXWeapon, Log, TEXT("%f, %f, %f"), Range, RangeModifier, ActualDamage);
	FEXPointDamageEvent PointDamageEvent = FEXPointDamageEvent(ActualDamage, HitResult, (HitResult.TraceEnd - HitResult.TraceStart).GetSafeNormal(), DamageType, GetOwningCharacter()->GetMercType());

	AEXCharacter* Target = Cast<AEXCharacter>(HitResult.GetActor());
	bool bAlive = Target && !Target->IsDead();
	float ResultingDamage = HitResult.GetActor()->TakeDamage(ActualDamage, PointDamageEvent, GetOwningCharacter()->GetController(), GetOwningCharacter());
	bool bKill = false;
	if (bAlive)
	{
		bKill = Target->IsDead();
	}
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
	bool bHeadShot = (SurfaceType == SURFACE_Head);
	if (PS)
	{
		PS->ShotHit(GetOwningCharacter()->GetMercType(), GetWeaponType(), bHeadShot, bKill, ResultingDamage);
	}
}

void UEXWeaponTrace::TraceNoHit(const FVector_NetQuantize TraceEnd)
{
	if (!IsAuthority())
	{
		Server_Unreliable_TraceNoHit(TraceEnd);
		return;
	}
	Multicast_PlayTraceEffects(TraceEnd);

	AEXPlayerState* PS = GetOwningCharacter()->GetPlayerState<AEXPlayerState>();
	if (PS)
	{
		PS->ShotMissed(GetOwningCharacter()->GetMercType(), GetWeaponType());
	}
}

void UEXWeaponTrace::PlayTraceEffects(const FVector HitLocation)
{
	if (TraceEffect)
	{
		UParticleSystemComponent* TracerComp = 
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, GetWeaponMeshComponent()->GetSocketTransform(MuzzleSocketName));
		if (TracerComp)
		{
			TracerComp->SetVectorParameter("BeamEnd", HitLocation);
		}
	}
}

void UEXWeaponTrace::Multicast_SpawnImpactEffect_Implementation(const FHitResult HitLocation)
{
	if (IsSimulatedProxy())
	{
		SpawnImpactEffect(HitLocation);
	}
}

void UEXWeaponTrace::Multicast_PlayTraceEffects_Implementation(const FVector HitLocation)
{
	if (IsSimulatedProxy())
	{
		PlayTraceEffects(HitLocation);
	}
}

FVector UEXWeaponTrace::GetTraceDirection(const FRotator ViewDirection) const
{
	if (Spread <= 0.f)
	{
		return ViewDirection.Vector();
	}
	FVector2D Point2 = FMath::RandPointInCircle(Spread);
	FVector Point(0.f, Point2.X, Point2.Y);
	FVector Offset = ViewDirection.RotateVector(Point);
	FVector TraceDirection = ViewDirection.Vector() + Offset;

	return TraceDirection;
}

