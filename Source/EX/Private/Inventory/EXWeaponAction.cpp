// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXWeaponAction.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXWeapon.h"

UEXWeaponAction::UEXWeaponAction()
{
}

void UEXWeaponAction::Initialize(UEXWeapon* Weapon, EWeaponInput Type)
{
	if (IsInitialized())
	{
		return;
	}

	OwningCharacter = Cast<AEXCharacter>(GetOwningActor());
	OwningWeapon = Weapon;
	OwningInventory = OwningCharacter->GetInventoryComponent();
	bInitialized = true;
	InputType = Type;
}

bool UEXWeaponAction::CanFire() const
{
	return GetOwningWeapon()->IsIdle();
}

bool UEXWeaponAction::Fire(float WorldTimeOverride)
{
	bHeldDown = true;
	if (!CanFire())
	{
		return false;
	}
	if (IsOwnedByLocalRemote())
	{
		Server_Reliable_Fire(WorldTimeOverride);
	}
	K2_OnFire();
	bFiring = true;
	return true;
}

void UEXWeaponAction::StopFire(float WorldTimeOverride)
{
	bHeldDown = false;
	if (IsOwnedByLocalRemote())
	{
		Server_Reliable_StopFire(WorldTimeOverride);
	}
	K2_OnStopFire();
}

void UEXWeaponAction::ToggleOff(float WorldTimeOverride)
{
	StopFire(WorldTimeOverride);
}

void UEXWeaponAction::FireComplete()
{
	K2_OnFireComplete();

	bFiring = false;
	GetOwningWeapon()->FireComplete(InputType);
}

void UEXWeaponAction::Cancel(float WorldTimeOverride)
{
	if (IsOwnedByLocalRemote())
	{
		Server_Reliable_Cancel(WorldTimeOverride);
	}

	GetOwningWeapon()->SetWeaponState(EWeaponState::Ready);
}

void UEXWeaponAction::Server_Reliable_Fire_Implementation(float WorldTimeOverride)
{
	Fire(WorldTimeOverride);
}

bool UEXWeaponAction::Server_Reliable_Fire_Validate(float WorldTimeOverride)
{
	return true;
}

void UEXWeaponAction::Server_Reliable_StopFire_Implementation(float WorldTimeOverride)
{
	StopFire(WorldTimeOverride);
}

bool UEXWeaponAction::Server_Reliable_StopFire_Validate(float WorldTimeOverride)
{
	return true;
}

void UEXWeaponAction::Server_Reliable_Cancel_Implementation(float WorldTimeOverride)
{
	Cancel(WorldTimeOverride);
}

bool UEXWeaponAction::Server_Reliable_Cancel_Validate(float WorldTimeOverride)
{
	return true;
}

float UEXWeaponAction::GetSpread(float WorldTimeOverride) const
{
	return 1.f;
}

