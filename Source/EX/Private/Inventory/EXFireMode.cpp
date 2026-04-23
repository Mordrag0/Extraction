// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXFireMode.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXWeapon.h"

UEXFireMode::UEXFireMode()
{
}

float UEXFireMode::GetSpreadTime(float WorldTimeOverride) const
{
	const float WorldTime = (WorldTimeOverride > 0) ? WorldTimeOverride : GetOwningCharacter()->GetWorld()->GetTimeSeconds();
	const float TimePassed = WorldTime - LastShotWorldTime;
	return FMath::Clamp(SpreadTimeAfterLastShot - TimePassed, MinSpreadTime, MaxSpreadTime);
}

float UEXFireMode::GetSpread(float WorldTimeOverride) const
{
	const float SpreadTime = GetSpreadTime(WorldTimeOverride);
	return Spread->GetFloatValue(SpreadTime);
}

bool UEXFireMode::Fire(float WorldTimeOverride)
{
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}
	if (IsLocallyOwned()) 
	{
		OwningWeapon->PerformShot();
	}
	OwningWeapon->ConsumeAmmo(AmmoPerShot);

	const float WorldTime = (WorldTimeOverride > 0) ? WorldTimeOverride : GetOwningCharacter()->GetWorld()->GetTimeSeconds();
	LastShotWorldTime = WorldTime + FireRate;
	SpreadTimeAfterLastShot = GetSpreadTime(WorldTimeOverride) + SpreadTimeAddedPerShot;

	K2_OnFire();
	return true;
}

void UEXFireMode::Initialize(UEXWeapon* Weapon, EWeaponInput Input)
{
	if (IsInitialized())
	{
		return;
	}
	Super::Initialize(Weapon, Input);
	Spread->GetTimeRange(MinSpreadTime, MaxSpreadTime);
}

bool UEXFireMode::CanFire() const
{
	return Super::CanFire() && OwningWeapon->HasEnoughAmmo(AmmoPerShot);
}

void UEXFireMode::FinishEarly()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	FireComplete();
}

float UEXFireMode::GetTimeRemaining() const
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Fire);
}

void UEXFireMode::FireComplete()
{
	Super::FireComplete();
}

