// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXAbilityTracker.h"
#include "Inventory/EXInventoryComponent.h"
#include "Player/EXCharacter.h"
#include "EX.h"

void UEXAbilityTracker::OnCharacterDeath(AEXCharacter* Character)
{
	ensure(Character->GetLocalRole() >= ROLE_AutonomousProxy);
	const EMerc MercType = Character->GetMercType();
	/*if (ensure(!Cooldown.Contains(MercType)))
	{
		FCooldownInfo& MercCooldowns = Cooldown.FindOrAdd(MercType);
		for (UEXInventory* Inventory : Character->GetInventoryComponent()->GetInventoryList())
		{
			if (!Inventory->IsAnAbility())
			{
				continue;
			}
			float& InventoryCooldown = MercCooldowns.Uses.FindOrAdd(Inventory->GetAbilityType());
			InventoryCooldown = Inventory->GetUses();
		}
	}*/
}

void UEXAbilityTracker::OnCharacterSpawned(AEXCharacter* Character)
{
	ensure(Character->GetLocalRole() >= ROLE_AutonomousProxy);
	const EMerc MercType = Character->GetMercType();
	//if (Cooldown.Contains(Character->GetType())) // Load cooldowns
	//{
	//	for (UEXInventory* Inventory : Character->GetInventoryComponent()->GetInventoryList())
	//	{
	//		if (!Inventory->IsAnAbility())
	//		{
	//			continue;
	//		}
	//		if (!Inventory->IsInitialized())
	//		{
	//			UE_LOG(LogEXInventory, Warning, TEXT("UEXAbilityTracker::OnCharacterSpawned Inventory not initialized"));
	//			continue;
	//		}
	//		EAbility AbilityType = Inventory->GetAbilityType();
	//		if (!Cooldown[MercType].Uses.Contains(AbilityType))
	//		{
	//			UE_LOG(LogEXInventory, Warning, TEXT("UEXAbilityTracker::OnCharacterSpawned Inventory not found"));
	//			continue;
	//		}
	//		Inventory->SetUses(Cooldown[MercType].Uses[AbilityType]);
	//	}
	//}
	//else // First spawn of this merc, so init cooldowns to 0
	//{
	//	for (UEXInventory* Inventory : Character->GetInventoryComponent()->GetInventoryList())
	//	{
	//		Inventory->SetUses(Inventory->GetMaxUses());
	//	}
	//}
}

void UEXAbilityTracker::AddAbility(AActor* Ability)
{
	Abilities.Add(Ability);
}

void UEXAbilityTracker::DestroyAbilities()
{
	for (AActor* Ability : Abilities)
	{
		if (IsValid(Ability))
		{
			Ability->Destroy(true);
		}
	}
}
