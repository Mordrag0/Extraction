// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXFireModeSemiAuto.h"
#include "EXTypes.h"
#include "Inventory/EXWeapon.h"
#include "EX.h"

bool UEXFireModeSemiAuto::Fire(float WorldTimeOverride)
{
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}
	OwningWeapon->SetWeaponState(EWeaponState::Firing);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &UEXFireModeSemiAuto::FireComplete, FireRate, false);
	return true;
}

bool UEXFireModeSemiAuto::CanFire() const
{
	if (GetOwningActor()->IsNetMode(NM_DedicatedServer))
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire)
			&& GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Fire) >= MAX_VARIANCE_ALLOWED)
		{
			return false;
		}
	}

	return Super::CanFire();
}

void UEXFireModeSemiAuto::FireComplete()
{
	OwningWeapon->SetWeaponState(EWeaponState::Ready);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	Super::FireComplete();
}
