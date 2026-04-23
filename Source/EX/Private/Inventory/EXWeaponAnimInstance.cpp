// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXWeaponAnimInstance.h"
#include "Inventory/EXInventory.h"

void UEXWeaponAnimInstance::Fire()
{
	bFire = true;
}

void UEXWeaponAnimInstance::Reload()
{
	bCancelReload = false;
	bReload = true;
}

void UEXWeaponAnimInstance::CancelReload()
{
	bCancelReload = true;
	bReload = false;
}

void UEXWeaponAnimInstance::WeaponStateChanged(EWeaponState WeaponState)
{
	switch (WeaponState)
	{
		case EWeaponState::Ready:
			CancelReload();
			break;
		case EWeaponState::Down:
			break;
		case EWeaponState::Equipping:
			break;
		case EWeaponState::Unequipping:
			break;
		case EWeaponState::Firing:
			Fire();
			break;
		case EWeaponState::Reloading:
			Reload();
			break;
		case EWeaponState::Holding:
			break;
		case EWeaponState::Max:
			break;
		default:
			break;
	}
}
