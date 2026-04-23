// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHUDAmmo.h"
#include "Components/TextBlock.h"
#include "Inventory/EXWeapon.h"
#include "Inventory/EXReloadAction.h"

void UEXHUDAmmo::UpdateAmmo(const UEXWeapon* Weapon)
{
	UEXReloadAction* ReloadAction = Weapon ? Weapon->GetReloadAction() : nullptr;
	if (ReloadAction)
	{
		const float WeaponMagAmmo = ReloadAction->GetMagAmmo();
		const float WeaponTotalAmmo = ReloadAction->GetSpareAmmo();

		Ammo->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(WeaponMagAmmo))));
		TotalAmmo->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(WeaponTotalAmmo))));
	}
	else
	{
		Ammo->SetText(EmptyText);
		TotalAmmo->SetText(EmptyText);
	}
}
