// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXStickyNadeInventory.h"
#include "Misc/EXStickyNade.h"
#include "Misc/EXStickyDetonator.h"
#include "Inventory/EXInventory.h"
#include "Inventory/EXInventoryComponent.h"

UEXStickyNadeInventory::UEXStickyNadeInventory()
{
	bShootOnFireReleased = true;
}

bool UEXStickyNadeInventory::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::Fire(Mode, WorldTimeOverride))
	{
		return false;
	}

	if ((Mode == EWeaponInput::Secondary) && IsLocallyOwned())
	{
		Detonator->QuickUse(Mode);
	}
	
	return true;
}

void UEXStickyNadeInventory::Init(AEXAbility* SpawnedAbility)
{
	Super::Init(SpawnedAbility); 

	AEXStickyNade* Sticky = CastChecked<AEXStickyNade>(SpawnedAbility);
	Detonator->AddSticky(Sticky);
}

void UEXStickyNadeInventory::PreInitialize()
{
	Super::PreInitialize();

	if (!!Detonator)
	{
		return;
	}

	UEXStickyDetonator* DetonatorInventory = InventoryOwner->GetInventory<UEXStickyDetonator>();
	if (DetonatorInventory)
	{
		Detonator = DetonatorInventory;
		Detonator->SetNadeInventory(this);
	}
}
