// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXStickyDetonator.h"
#include "Misc/EXStickyNadeInventory.h"
#include "Misc/EXStickyNade.h"
#include "Inventory/EXInventoryComponent.h"

bool UEXStickyDetonator::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::Fire(Mode, WorldTimeOverride))
	{
		return false;
	}

	if (Mode != EWeaponInput::Primary)
	{
		return false;
	}
	for (AEXStickyNade* Sticky : Stickies)
	{
		if (Sticky)
		{
			Sticky->Explode();
		}
	}
	Stickies.Empty();
	return true;
}

void UEXStickyDetonator::PreInitialize()
{
	Super::PreInitialize();

	if (!!NadeInventory)
	{
		return;
	}

	TArray<UEXInventory*> InventoryList = InventoryOwner->GetInventoryList();
	for (UEXInventory* Inventory : InventoryList)
	{
		UEXStickyNadeInventory* StickyNadeInventory = Cast<UEXStickyNadeInventory>(Inventory);
		if (StickyNadeInventory)
		{
			NadeInventory = StickyNadeInventory;
			NadeInventory->SetDetonator(this);
		}
	}
}

