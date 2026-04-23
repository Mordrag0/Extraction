// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXInventoryInteract.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"

bool UEXInventoryInteract::Unequip()
{
	if (!Super::Unequip())
	{
		return false;
	}

	GetOwningCharacter()->StopInteract(true);
	return true;
}

void UEXInventoryInteract::EquipComplete()
{
	Super::EquipComplete();

	if (IsAuthority())
	{
		InventoryOwner->EngiToolEquipped();
	}
}

void UEXInventoryInteract::UnequipComplete()
{
	Super::UnequipComplete();

	if (IsAuthority())
	{
		InventoryOwner->ClearInteractObjective();
	}
}

void UEXInventoryInteract::BeginPlay()
{
	SetProgressModifier(DefaultProgressModifier);
	Super::BeginPlay();
}
