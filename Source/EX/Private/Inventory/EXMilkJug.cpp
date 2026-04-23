// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXMilkJug.h"
#include "Misc/EXCarryableObjective.h"
#include "Player/EXCharacter.h"

UEXMilkJug::UEXMilkJug()
{
}

void UEXMilkJug::SetJugActor(AEXCarryableObjective* MilkJug)
{
	MilkJugActor = MilkJug;
}

bool UEXMilkJug::Unequip()
{
	if (!Super::Unequip())
	{
		return false;
	}
	GetOwningCharacter()->StopInteract(true);
	return true;
}

void UEXMilkJug::UnequipComplete()
{
	Super::UnequipComplete();

	if (IsAuthority())
	{
		if (IsValid(MilkJugActor))
		{
			if (MilkJugActor)
			{
				MilkJugActor->OnDropped(GetOwningCharacter()->GetActorLocation(), GetOwningCharacter()->GetActorRotation());
			}
			else
			{
				UE_LOG(LogEXWeapon, Error, TEXT("Missing milkjug actor"));
			}
		}
	}
	MilkJugActor = nullptr;
}

void UEXMilkJug::EquipComplete()
{
	Super::EquipComplete();

	// When its equipped we're not interacting
	// And we're also making sure that we're not using its movement multiplier, because it would prevent sprinting
	GetOwningCharacter()->StopInteract(MilkJugActor, true, true);
}

bool UEXMilkJug::CanEquip() const
{
	if (!Super::CanEquip())
	{
		return false;
	}
	return !!MilkJugActor;
}
