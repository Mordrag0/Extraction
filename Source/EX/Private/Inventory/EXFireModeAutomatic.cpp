// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXFireModeAutomatic.h"
#include "Inventory/EXWeapon.h"
#include "EX.h"
#include "Player/EXCharacter.h"

bool UEXFireModeAutomatic::Fire(float WorldTimeOverride)
{
	if (IsLocallyOwned() && GetOwningWeapon()->IsFiring())
	{
		// Quickly letting go and refiring - handle as if the player never stopped firing
		bHeldDown = true;
		return true;
	}
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}

	GetOwningWeapon()->SetWeaponState(EWeaponState::Firing);
	if (!IsLocallyOwned())
	{
		// On server the timer will be restarted for each shot individually
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &UEXFireModeAutomatic::FireComplete, FireRate, false);
	}
	else if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire))
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &UEXFireModeAutomatic::FireComplete, FireRate, true);
	}
	return true;
}

void UEXFireModeAutomatic::FireComplete()
{
	const float WorldTime = GetWorld()->GetTimeSeconds();
	OwningWeapon->SetWeaponState(EWeaponState::Ready);
	const bool bContinuingFire =
		IsLocallyOwned()
		&& IsHeldDown()
		&& CanFire()
		&& !GetOwningWeapon()->IsUnequipping()
		&& !GetOwningWeapon()->IsPendingUnequip()
		&& !GetOwningCharacter()->IsDead();

	if (!bContinuingFire)
	{
		Super::FireComplete();
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
		const bool bHolding = IsHeldDown();
		StopFire(WorldTime);

		if (bHolding && !OwningWeapon->HasEnoughAmmo(AmmoPerShot))
		{
			OwningWeapon->OnOutOfAmmo();
		}
	}
	K2_OnFireComplete();
	GetOwningWeapon()->FireComplete(InputType);

	if (!bContinuingFire)
	{
		return;
	}
	// If unequipping or no longer holding fire, stop firing
	// Also stop if we can't fire (for example no ammo) except if gun is 
	// firing (player could release LMB and press it again before weapon is done firing
	// in that case it should be handled as if he never let go of it

	const bool bFired = GetOwningWeapon()->Fire(InputType, WorldTime);
	ensure(bFired); // bFired should be true, because we checked CanFire() above
}

void UEXFireModeAutomatic::StopFire(float WorldTimeOverride)
{
	Super::StopFire(WorldTimeOverride);
}

bool UEXFireModeAutomatic::CanFire() const
{
	if (IsAuthority())
	{
		const FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(TimerHandle_Fire)
			&& TimerManager.GetTimerRemaining(TimerHandle_Fire) >= MAX_VARIANCE_ALLOWED)
		{
			return false;
		}
	}
	return Super::CanFire();
}
