// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXPersistentUser.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Player/EXLocalPlayer.h"
#include "HUD/EXOptionsInput.h"
#include "System/EXGameplayStatics.h"
#include "Misc/EXInputInfo.h"

UEXPersistentUser::UEXPersistentUser()
{
	SetToDefaults();
}

void UEXPersistentUser::SaveIfDirty()
{
	if (bIsDirty)
	{
		SavePersistenUser();
	}
}

void UEXPersistentUser::SetInvertedYAxis(bool bInvert)
{
	bIsDirty |= bInvertedYAxis != bInvert;

	bInvertedYAxis = bInvert;
}

void UEXPersistentUser::SetAimSensitivity(float InSensitivity)
{
	bIsDirty |= AimSensitivity != InSensitivity;

	AimSensitivity = InSensitivity;
}

void UEXPersistentUser::SetScopeSensitivityScale(float InScopeSensitivityScale)
{
	bIsDirty |= ScopeSensitivityScale != InScopeSensitivityScale;

	ScopeSensitivityScale = InScopeSensitivityScale;
}

void UEXPersistentUser::SetGamma(float InGamma)
{
	bIsDirty |= Gamma != InGamma;

	Gamma = InGamma;
}

void UEXPersistentUser::SetChangedInputs(TMap<FMyInputId, FKey> InChangedInputs)
{
	for (const TTuple<FMyInputId, FKey>& Bind : InChangedInputs)
	{
		ChangedInputs.FindOrAdd(Bind.Key) = Bind.Value;
	}
	bIsDirty = true;
}

void UEXPersistentUser::SetMasterVolume(float InMasterVolume)
{
	bIsDirty |= MasterVolume != InMasterVolume;

	MasterVolume = FMath::Clamp(InMasterVolume, 0.f, 1.f);
}

void UEXPersistentUser::SetMuted(bool bInMuted)
{
	bIsDirty |= bMuted != bInMuted;
	bMuted = bInMuted;
}

void UEXPersistentUser::SetMusicVolume(float InMusicVolume)
{
	bIsDirty |= MusicVolume != InMusicVolume;

	MusicVolume = FMath::Clamp(InMusicVolume, 0.f, 1.f);
}

void UEXPersistentUser::SetFOV(float InFOV)
{
	bIsDirty |= FOV != InFOV;

	FOV = InFOV;
}

void UEXPersistentUser::SetHoldObjective(bool bInHoldObjective)
{
	bIsDirty |= bHoldObjective != bInHoldObjective;

	bHoldObjective = bInHoldObjective;
}

void UEXPersistentUser::SetHitMarkers(bool bInHitMarkers)
{
	bIsDirty |= bHitMarkers != bInHitMarkers;

	bHitMarkers = bInHitMarkers;
}

void UEXPersistentUser::SetToggleSprint(bool bInToggleSprint)
{
	bIsDirty |= bToggleSprint != bInToggleSprint;

	bToggleSprint = bInToggleSprint;
}

void UEXPersistentUser::SetToggleWalk(bool bInToggleWalk)
{
	bIsDirty |= bToggleWalk != bInToggleWalk;

	bToggleWalk = bInToggleWalk;
}

void UEXPersistentUser::SetToggleADS(bool bInToggleADS)
{
	bIsDirty |= bToggleADS != bInToggleADS;

	bToggleADS = bInToggleADS;
}

void UEXPersistentUser::SetADSCrosshair(bool bInADSCrosshair)
{
	bIsDirty |= bADSCrosshair != bInADSCrosshair;

	bADSCrosshair = bInADSCrosshair;
}

void UEXPersistentUser::SetReloadCancelsReload(bool bInReloadCancelsReload)
{
	bIsDirty |= bReloadCancelsReload != bInReloadCancelsReload;

	bReloadCancelsReload = bInReloadCancelsReload;
}

void UEXPersistentUser::SetCrosshairs(const TArray<FCrosshair>& InCrosshairs)
{
	bool ChangedNum = Crosshairs.Num() != InCrosshairs.Num();
	bIsDirty |= ChangedNum;

	if (!ChangedNum)
	{
		for (int32 Idx = 0; Idx < Crosshairs.Num(); Idx++)
		{
			bIsDirty |= Crosshairs[Idx] != InCrosshairs[Idx];
		}
	}

	Crosshairs = InCrosshairs;
}

void UEXPersistentUser::SetActiveCrosshairIdx(uint32 InActiveCrosshairIdx)
{
	bIsDirty |= ActiveCrosshairIdx != InActiveCrosshairIdx;

	ActiveCrosshairIdx = InActiveCrosshairIdx;
}

void UEXPersistentUser::SetToDefaults()
{
	bIsDirty = false;

	bInvertedYAxis = false;
	AimSensitivity = 1.0f;
	ScopeSensitivityScale = 0.f;
	ChangedInputs.Empty();
	MasterVolume = 1.f;
	bMuted = false;
	MusicVolume = 1.f;
	FOV = 80.f;
	bHitMarkers = true;
	bHoldObjective = true;
	bToggleSprint = false;
	bToggleADS = false;
	bADSCrosshair = true;
	bReloadCancelsReload = false;
	bToggleWalk = false;
	Gamma = 2.2;

	Crosshairs.Empty(1);
	Crosshairs.Add(FCrosshair());
}

void UEXPersistentUser::SavePersistenUser()
{
	UGameplayStatics::SaveGameToSlot(this, SlotName, UserIndex);
	bIsDirty = false;
}

UEXPersistentUser* UEXPersistentUser::LoadPersistentUser(FString SlotName, const int32 UserIndex)
{
	UEXPersistentUser* Result = nullptr;

	// First set of player signins can happen before the UWorld exists, which means no OSS, which means no user names, which means no slotnames.
	// Persistent users aren't valid in this state.
	if (SlotName.Len() > 0)
	{
		if (!GIsBuildMachine && UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
		{
			Result = Cast<UEXPersistentUser>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
		}

		if (!Result)
		{
			// If failed to load, create a new one
			Result = Cast<UEXPersistentUser>(UGameplayStatics::CreateSaveGameObject(UEXPersistentUser::StaticClass()));
		}
		check(Result);

		Result->SlotName = SlotName;
		Result->UserIndex = UserIndex;
	}
	return Result;
}


void UEXPersistentUser::TellInputAboutKeybindings()
{
	TArray<APlayerController*> PlayerList;
	GEngine->GetAllLocalPlayerControllers(PlayerList);
	if (PlayerList.Num() == 0)
	{
		return;
	}
	APlayerController* PC = PlayerList[0];
	if (!PC || !PC->Player || !PC->PlayerInput)
	{
		return;
	}
		
	// Update key bindings for the current user only
	UEXLocalPlayer* LocalPlayer = Cast<UEXLocalPlayer>(PC->Player);
	if (!LocalPlayer || LocalPlayer->GetPersistentUser() != this)
	{
		return;
	}

	// #Bindings
	// Set the aim sensitivity
	for (FInputAxisKeyMapping& AxisMapping : PC->PlayerInput->AxisMappings)
	{
		if (AxisMapping.AxisName == "Lookup" || AxisMapping.AxisName == "Turn")
		{
			AxisMapping.Scale = (AxisMapping.Scale < 0.0f) ? -GetAimSensitivity() : +GetAimSensitivity();
		}
		for (const TTuple<FMyInputId, FKey>& Input : ChangedInputs)
		{
			if (AxisMapping.AxisName.IsEqual(Input.Key.Name) && ((AxisMapping.Scale < 0) == Input.Key.bInverted))
			{
				AxisMapping.Key = Input.Value;
			}
		}
	}
	for (FInputActionKeyMapping& ActionMapping : PC->PlayerInput->ActionMappings)
	{
		for (const TTuple<FMyInputId, FKey>& Input : ChangedInputs)
		{
			if (ActionMapping.ActionName.IsEqual(Input.Key.Name))
			{
				ActionMapping.Key = Input.Value;
			}
		}
	}
	PC->PlayerInput->ForceRebuildingKeyMaps();

	if (PC->PlayerInput->GetInvertAxis("Lookup") != GetInvertedYAxis())
	{
		PC->PlayerInput->InvertAxis("Lookup");
	}
	UpdateInputBindingInfo(PC);
}

void UEXPersistentUser::UpdateInputBindingInfo(APlayerController* PC)
{
	UEXInputInfo* InputInfo = UEXGameplayStatics::GetInputInfoCDO();
	TMap<FMyInputId, FInputBindValue>& Binds = InputInfo->Inputs;

	for (TPair<FMyInputId, FInputBindValue>& Bind : Binds)
	{
		FInputAxisKeyMapping* AxisMapping =
			PC->PlayerInput->AxisMappings.FindByPredicate([Bind](const FInputAxisKeyMapping& Other)
				{
					return (Other.AxisName.IsEqual(Bind.Key.Name) && ((Other.Scale < 0) == Bind.Key.bInverted));
				}
		);
		if (AxisMapping)
		{
			Bind.Value.Key = AxisMapping->Key;
		}
		else
		{
			FInputActionKeyMapping* ActionMapping =
				PC->PlayerInput->ActionMappings.FindByPredicate([Bind](const FInputActionKeyMapping& Other)
					{
						return Other.ActionName.IsEqual(Bind.Key.Name);
					}
			);
			if (ActionMapping)
			{
				Bind.Value.Key = ActionMapping->Key;
			}
		}
	}
	InputInfo->OnInputsUpdated.Broadcast();
}

