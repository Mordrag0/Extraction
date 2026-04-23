// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXSpawnableAbility.h"
#include "Inventory/EXInventory.h"
#include "Player/EXCharacter.h"
#include "Misc/EXPlacedAbility.h"
#include "Misc/EXPlacingAbility.h"
#include "Player/EXPlayerController.h"
#include "Online/EXLevelRules.h"
#include "Inventory/EXAbilityTracker.h"

UEXSpawnableAbility::UEXSpawnableAbility()
{
	bUnequipAbilityWhenEmpty = true;
	QuickUseType = EQuickUseType::Use;
}

bool UEXSpawnableAbility::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (Mode != EWeaponInput::Primary)
	{
		return false;
	}
	if (!CanFire(Mode))
	{
		return false;
	}
	
	if (TemporaryAbility)
	{
		Cancel();
		check(0 && "Ability not cleared properly");
	}
	TemporaryAbility = GetWorld()->SpawnActor<AEXPlacingAbility>(TemporaryAbilityClass);
	TemporaryAbility->SetCharacter(GetOwningCharacter());
	SetWeaponState(EWeaponState::Firing);
	
	return true;
}

bool UEXSpawnableAbility::StopFire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (Mode != EWeaponInput::Primary)
	{
		return false;
	}
	if (TemporaryAbility)
	{
		const bool bValidPlacement = TemporaryAbility->IsValidPlacement();
		if (bValidPlacement)
		{
			const FVector Loc = TemporaryAbility->GetActorLocation();
			const FRotator Rot = TemporaryAbility->GetActorRotation();
			Server_Reliable_SpawnAbility(Loc, Rot);
			SetWeaponState(EWeaponState::Ready);
		}
		TemporaryAbility->Destroy();
		TemporaryAbility = nullptr;
		return bValidPlacement;
	}
	return true;
}

void UEXSpawnableAbility::Server_Reliable_PlaceAbility_Implementation()
{
	SetWeaponState(EWeaponState::Firing);
}

bool UEXSpawnableAbility::Server_Reliable_PlaceAbility_Validate()
{
	return true;
}

bool UEXSpawnableAbility::CanFire(EWeaponInput Mode) const
{
	return Super::CanFire(Mode) && CanUse();
}

void UEXSpawnableAbility::Cancel()
{
	if (TemporaryAbility)
	{
		TemporaryAbility->Destroy();
	}
}

void UEXSpawnableAbility::Server_Reliable_SpawnAbility_Implementation(const FVector Location, const FRotator Rotation)
{
	if (IsFiring())
	{
		Use();
		if (AbilityClass)
		{
			if (SpawnedAbilities.Num() >= MaxCount)
			{
				SpawnedAbilities[0]->NetDestroy();
				SpawnedAbilities.RemoveAt(0);
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwningCharacter()->GetEXController();
			SpawnParams.Instigator = GetOwningCharacter();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AEXPlacedAbility* SpawnedAbility = GetWorld()->SpawnActor<AEXPlacedAbility>(AbilityClass, Location, Rotation, SpawnParams);
			SpawnedAbility->Init(this);
			if (UEXAbilityTracker* AbilityTracker = GetOwningCharacter()->GetEXController()->GetAbilityTracker())
			{
				AbilityTracker->AddAbility(SpawnedAbility);
			}
			SpawnedAbilities.Add(SpawnedAbility);
		}
		SetWeaponState(EWeaponState::Ready);
	}
}

bool UEXSpawnableAbility::Server_Reliable_SpawnAbility_Validate(const FVector Location, const FRotator Rotation)
{
	return true;
}
