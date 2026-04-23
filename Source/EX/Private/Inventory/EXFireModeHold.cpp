// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXFireModeHold.h"
#include "Inventory/EXWeapon.h"

UEXFireModeHold::UEXFireModeHold()
{
}

bool UEXFireModeHold::Fire(float WorldTimeOverride)
{
	Super::Fire(WorldTimeOverride);
	OwningWeapon->SetWeaponState(EWeaponState::Holding);
	return true;
}

void UEXFireModeHold::StopFire(float WorldTimeOverride)
{
	Super::StopFire(WorldTimeOverride);
	OwningWeapon->SetWeaponState(EWeaponState::Firing);
	if (IsLocallyOwned())
	{
		// #EXTODO2
		//OwningWeapon->PerformShot();
	}
}

