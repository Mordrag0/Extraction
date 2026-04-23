// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/EXADSAction.h"
#include "Player/EXPlayerController.h"
#include "Player/EXPlayerCameraManager.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXWeapon.h"
#include "Player/EXAnimInstance.h"
#include "Inventory/EXInventoryComponent.h"

void UEXADSAction::StartADS()
{
	bADSing = true;
	GetOwningWeapon()->SetFireModeSpreadModifier(SpreadModifier);
	if (PCM)
	{
		PCM->SetTargetFOV(ADS_FOV);
	}
 
	if (bScope)
	{
		GetOwningCharacter()->StartScoping();
	}
	else
	{
		GetOwningCharacter()->StartADSing();
	}
}

void UEXADSAction::StopADS()
{
	FireComplete();
	if (!bADSing)
	{
		// There are multiple reasons why we would stop ADSing early
		return;
	}
	bADSing = false;
	GetOwningWeapon()->SetFireModeSpreadModifier(1.f);
	if (PCM)
	{
		PCM->ResetFOV();
	}

	if (bScope)
	{
		GetOwningCharacter()->StopScoping();
	}
	else
	{
		GetOwningCharacter()->StopADSing();
	}
}

bool UEXADSAction::Fire(float WorldTimeOverride)
{
	if (!Super::Fire(WorldTimeOverride))
	{
		return false;
	}
	StartADS();
	return true;
}

void UEXADSAction::StopFire(float WorldTimeOverride)
{
	Super::StopFire(WorldTimeOverride);
	StopADS();
}

void UEXADSAction::Initialize(UEXWeapon* Weapon, EWeaponInput Type)
{
	if (IsInitialized())
	{
		return;
	}
	Super::Initialize(Weapon, Type);

	if (!IsSimulatedProxy())
	{
		InitPCM();
	}
	if (IsLocallyOwned() && OwningCharacter)
	{
		bToggled = OwningCharacter->GetInventoryComponent()->GetToggleADS();
	}
}

void UEXADSAction::InitPCM()
{
	AEXPlayerController* PC = OwningCharacter->GetController<AEXPlayerController>();
	PCM = PC ? PC->GetPCM() : nullptr;
	if (PC && !PCM)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't get PCM"));
	}
}

