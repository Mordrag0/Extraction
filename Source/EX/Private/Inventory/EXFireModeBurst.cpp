// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXFireModeBurst.h"
#include "Inventory/EXWeapon.h"
#include "EX.h"

bool UEXFireModeBurst::Fire(float WorldTimeOverride)
{
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}
	GetOwningWeapon()->SetWeaponState(EWeaponState::Firing);
	if (IsLocallyOwned())
	{
		if(!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Burst))
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Burst, this, &UEXFireModeBurst::BurstComplete, TimeBetweenBursts, false);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &UEXFireModeBurst::FireComplete, FireRate, true);
			RemainingBurst = ShotsPerBurst;
		}
	}
	else if (IsAuthority())
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Burst)
			|| GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Burst) < MAX_VARIANCE_ALLOWED)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Burst, this, &UEXFireModeBurst::BurstComplete, TimeBetweenBursts, false);
			RemainingBurst = ShotsPerBurst;
		}
	}
	--RemainingBurst;
	return true;
}

void UEXFireModeBurst::StopFire(float WorldTimeOverride)
{
	return Super::StopFire(WorldTimeOverride);
}

bool UEXFireModeBurst::CanFire() const
{
	const FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (IsLocallyOwned())
	{
		// If burst is still active but no more shots in this burst, then we cant shoot
		if (TimerManager.IsTimerActive(TimerHandle_Burst) && (RemainingBurst <= 0))
		{
			return false;
		}
	}
	else if (IsAuthority()) 
	{
		if (TimerManager.IsTimerActive(TimerHandle_Burst)
			&& (TimerManager.GetTimerRemaining(TimerHandle_Burst) >= MAX_VARIANCE_ALLOWED))
		{
			if (RemainingBurst <= 0)
			{
				return false;
			}
		}
		if (TimerManager.IsTimerActive(TimerHandle_Fire)
			&& (TimerManager.GetTimerRemaining(TimerHandle_Fire) >= MAX_VARIANCE_ALLOWED))
		{
			return false;
		}
	}
	// This checks bFiring, which is set to false in FireComplete
	// So we can shoot if burst isn't active or its time for the next shot in the burst
	return Super::CanFire();
}

void UEXFireModeBurst::FinishEarly()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Burst);
	Super::FinishEarly();
}

void UEXFireModeBurst::FireComplete()
{
	if (RemainingBurst > 0)
	{
		K2_OnFireComplete();

		if (IsLocallyOwned())
		{
			float WorldTime = GetWorld()->GetTimeSeconds();
			bool bFired = GetOwningWeapon()->Fire(InputType, WorldTime);
			if (!bFired)
			{
				// Reset Burst
				ResetBurst();
				Super::FireComplete();
			}
		}
	}
	else // Skip calling fire complete on weapon (through Super::FireComplete()), because it will set the state to ready
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	}
}

void UEXFireModeBurst::ResetBurst()
{
	if (IsOwnedByLocalRemote())
	{
		Server_Reliable_ResetBusrst();
	}
	RemainingBurst = 0;
}


void UEXFireModeBurst::Server_Reliable_ResetBusrst_Implementation()
{
	ResetBurst();
}

bool UEXFireModeBurst::Server_Reliable_ResetBusrst_Validate()
{
	return true;
}

void UEXFireModeBurst::BurstComplete()
{
	GetOwningWeapon()->SetWeaponState(EWeaponState::Ready);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Burst);
	Super::FireComplete();
}
