// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXReloadAction.h"
#include "Inventory/EXWeapon.h"
#include "Net/UnrealNetwork.h"
#include "EXNetDefines.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Inventory/EXADSAction.h"

void UEXReloadAction::Initialize(UEXWeapon* Weapon, EWeaponInput Type)
{
	Super::Initialize(Weapon, Type);

	MagAmmo = MaxMagAmmo;
	SpareAmmo = MaxAmmo - MaxMagAmmo;

}

bool UEXReloadAction::Fire(float WorldTimeOverride)
{
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Reload, this, &UEXReloadAction::ReloadComplete, FireRate);
	GetOwningWeapon()->SetWeaponState(EWeaponState::Reloading);

	if (UEXADSAction* ADSAction = GetOwningWeapon()->GetADSAction())
	{
		ADSAction->StopADS();
	}

	return true;
}

void UEXReloadAction::Cancel(float WorldTimeOverride)
{
	bHeldDown = false;
	bFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Reload);
	GetOwningCharacter()->CancelReloadAnim();
	Super::Cancel(WorldTimeOverride);
}

void UEXReloadAction::FinishEarly()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Reload);
	ReloadComplete();
}

bool UEXReloadAction::CanFire() const
{
	if (!GetOwningWeapon()->IsIdle() || GetOwningWeapon()->IsFiring())
	{
		return false;
	}

	if (MagAmmo >= MaxMagAmmo) // Mag already full
	{
		return false;
	}
	if (SpareAmmo == 0) // No ammo to reload
	{
		return false;
	}
	return true;
}

int32 UEXReloadAction::AddAmmo(int32 Mags)
{
	const uint8 OldSpareAmmo = SpareAmmo;
	SpareAmmo = FMath::Min(SpareAmmo + Mags * MaxMagAmmo, MaxAmmo - MagAmmo);
	MARK_PROPERTY_DIRTY_FROM_NAME(UEXReloadAction, SpareAmmo, this);
	return FMath::CeilToInt32(((float)(SpareAmmo - OldSpareAmmo)) / MaxMagAmmo); // How many mags of ammo were added, rounded up
}

bool UEXReloadAction::IsLowOnAmmo() const
{
	return GetTotalAmmo() < (MaxMagAmmo * 2);
}

bool UEXReloadAction::IsMagLowOnAmmo() const
{
	return (MagAmmo <= MaxMagAmmo * 0.2f);
}

bool UEXReloadAction::IsAmmoFull() const
{
	return MagAmmo + SpareAmmo == MaxAmmo;
}

void UEXReloadAction::ConsumeAmmo(uint8 Amount)
{
	const bool bLowMagAmmo = IsMagLowOnAmmo();
	MagAmmo -= Amount;
	if (OwningCharacter->HasAuthority())
	{
		OwningInventory->RecheckLowAmmo();
	}
	if (IsLocallyOwned())
	{
		GetOwningWeapon()->UpdateHUD();
		if (!bLowMagAmmo && IsMagLowOnAmmo()) // We weren't low before this function call but now we are
		{
			OnMagLowOnAmmo();
		}
	}
}

void UEXReloadAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_WITH_PARAMS_FAST(UEXReloadAction, SpareAmmo, PushReplicationParams::OwnerOnly);
}

float UEXReloadAction::GetTimeRemaining() const
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Reload);
}

void UEXReloadAction::StopFire(float WorldTimeOverride)
{
	// Don't call super to avoid calling RPC
	bHeldDown = false;
	K2_OnStopFire();
}

void UEXReloadAction::ToggleOff(float WorldTimeOverride)
{
	if (bToggled) 
	{
		Cancel(WorldTimeOverride);
	}
}

void UEXReloadAction::OnMagLowOnAmmo()
{
	if (LowAmmoBeep)
	{
		UGameplayStatics::PlaySound2D(this, LowAmmoBeep);
	}
}

void UEXReloadAction::ReloadComplete()
{
	bFiring = false;
	const uint8 OldMagAmmo = MagAmmo;
	MagAmmo = FMath::Min(MaxMagAmmo, (uint8)(MagAmmo + SpareAmmo));
	SpareAmmo -= MagAmmo - OldMagAmmo;
	MARK_PROPERTY_DIRTY_FROM_NAME(UEXReloadAction, SpareAmmo, this);
	GetOwningWeapon()->SetWeaponState(EWeaponState::Ready);
	GetOwningWeapon()->UpdateHUD();
}

void UEXReloadAction::OnRep_SpareAmmo()
{
	if (IsInitialized())
	{
		// Just making sure we're not going over the top in case a reload also happened right about the time this value was updated
		SpareAmmo = FMath::Min(SpareAmmo, (uint8)(MaxAmmo - MagAmmo));
		GetOwningWeapon()->UpdateHUD();
	}
}

