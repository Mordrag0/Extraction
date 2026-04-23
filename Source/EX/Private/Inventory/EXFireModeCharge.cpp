// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXFireModeCharge.h"
#include "Inventory/EXWeapon.h"
#include "Inventory/EXWeaponTraceCharge.h"


bool UEXFireModeCharge::Fire(float WorldTimeOverride)
{
	if (!CanFire())
	{
		return false;
	}
	OwningWeapon->SetWeaponState(EWeaponState::Holding);
	K2_OnFire();
	return true;
}

void UEXFireModeCharge::StopFire(float WorldTimeOverride)
{
	Super::StopFire(WorldTimeOverride);

	OwningWeapon->SetWeaponState(EWeaponState::Firing);
	OwningWeapon->PerformShot();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &UEXFireModeCharge::FireComplete, FireRate, false);
}

bool UEXFireModeCharge::CanFire() const
{
	return Super::CanFire();
}

void UEXFireModeCharge::FireComplete()
{
	OwningWeapon->SetWeaponState(EWeaponState::Ready);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	Super::FireComplete();
}
