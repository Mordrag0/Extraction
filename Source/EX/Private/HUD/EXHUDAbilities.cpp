// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHUDAbilities.h"
#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXInventory.h"
#include "HUD/EXAbilityIcon.h"

void UEXHUDAbilities::Init(UEXInventoryComponent* InventoryComponent)
{
	int32 Idx = 0;
	// Init the icons that we use
	for (const UEXInventory* Inventory : InventoryComponent->GetInventoryList())
	{
		if (!Inventory->IsAnAbility())
		{
			continue;
		}
		check(IconArray.IsValidIndex(Idx));
		UEXAbilityIcon* Icon = IconArray[Idx];
		AbilityIcons.Add(Inventory, Icon);
		Icon->InitIcon(Inventory, Inventory->GetIcon(EIconSize::Small).Get());
		Icon->ShowIcon();
		Idx++;
	}

	// Hide the rest of the icons
	for (; Idx < IconArray.Num(); Idx++)
	{
		IconArray[Idx]->HideIcon();
	}
}

void UEXHUDAbilities::UpdateInventory(const UEXInventory* Inventory)
{
	if (AbilityIcons.Contains(Inventory))
	{
		AbilityIcons[Inventory]->UpdateInventory();
	}
}

void UEXHUDAbilities::NativeConstruct()
{
	Super::NativeConstruct();

	AbilityIcons.Empty(3);
	IconArray.Empty(3);
	IconArray.Add(AbilityIcon1);
	IconArray.Add(AbilityIcon2);
	IconArray.Add(AbilityIcon3);
}
