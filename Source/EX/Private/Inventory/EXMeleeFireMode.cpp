// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXMeleeFireMode.h"
#include "EXTypes.h"
#include "Inventory/EXWeapon.h"

bool UEXMeleeFireMode::Fire(float WorldTimeOverride)
{
	if (!CanFire())
	{
		return false;
	}
	K2_OnFire();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &UEXMeleeFireMode::FireComplete, FireRate, false);
	OwningWeapon->SetWeaponState(EWeaponState::Firing);
	return true;
}

bool UEXMeleeFireMode::CanFire() const
{
	if (IsAuthority())
	{
		return true;
	}
	return GetOwningWeapon()->CanFire(InputType); 
}

void UEXMeleeFireMode::FireComplete()
{
	OwningWeapon->SetWeaponState(EWeaponState::Ready);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	Super::FireComplete();
}

