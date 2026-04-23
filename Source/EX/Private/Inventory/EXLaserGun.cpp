// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXLaserGun.h"
#include "EX.h"
#include "Misc/EXLaser.h"
#include "Player/EXCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/EXFireMode.h"

UEXLaserGun::UEXLaserGun()
{
	bUsesAbility = true;
}

void UEXLaserGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UEXLaserGun, Laser, COND_OwnerOnly);
}

void UEXLaserGun::PerformShot()
{
	if (!IsLocallyOwned())
	{
		return;
	}
	FVector Location;
	if (GetTargetLocation(Location))
	{
		MoveLaser(Location);
		Server_Reliable_MoveLaser(Location);
	}
}

bool UEXLaserGun::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::Fire(Mode, WorldTimeOverride))
	{
		return false;
	}

	if (!Cast<UEXFireMode>(Actions[Mode]))
	{
		return true;
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnLaser, this, &UEXLaserGun::SpawnLaser, TimeToSpawnLaser, false);
	return true;
}

bool UEXLaserGun::StopFire(EWeaponInput Mode, float WorldTimeOverride)
{
	Super::StopFire(Mode, WorldTimeOverride);
	if (!Cast<UEXFireMode>(Actions[Mode]))
	{
		return true;
	}
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpawnLaser);
	return true;
}

bool UEXLaserGun::CanFire(EWeaponInput Mode) const
{
	return Super::CanFire(Mode) && CanUse(1.f);
}

void UEXLaserGun::Server_Reliable_MoveLaser_Implementation(FVector Location)
{
	MoveLaser(Location);
}

void UEXLaserGun::Server_Reliable_SpawnLaser_Implementation(FVector Location)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = GetOwningCharacter();
	SpawnParams.Owner = GetOwningCharacter();
	Laser = GetWorld()->SpawnActor<AEXLaser>(LaserClass, Location, FRotator::ZeroRotator, SpawnParams);
}

bool UEXLaserGun::GetTargetLocation(FVector& Locaction)
{
	FVector Loc;
	FRotator Rot;
	GetOwningCharacter()->GetActorEyesViewPoint(Loc, Rot);
	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.AddIgnoredActor(GetOwningCharacter());

	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Loc, Loc + Rot.Vector() * TraceDistance, ECC_Visibility_Simple, CQP) || !HitResult.GetActor())
	{
		Locaction = HitResult.ImpactPoint;
		return true;
	}
	return false;
}

void UEXLaserGun::SpawnLaser()
{
	if (!IsLocallyOwned())
	{
		return;
	}
	FVector Location;
	if (GetTargetLocation(Location))
	{
		Server_Reliable_SpawnLaser(Location);
	}
	
}

void UEXLaserGun::MoveLaser(FVector Location)
{
	if (IsValid(Laser))
	{
		Laser->SetTargetLocation(Location);
	}
}
