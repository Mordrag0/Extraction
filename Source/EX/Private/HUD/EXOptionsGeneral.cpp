// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptionsGeneral.h"
#include "System/EXSettings.h"
#include "Components/CheckBox.h"


void UEXOptionsGeneral::Init(TSharedPtr<FEXSettings> InSettings, bool bReset)
{
	Settings = InSettings;

	HitMarkers->SetCheckedState(Settings->GetHitMarkers() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	HitMarkers->OnCheckStateChanged.AddDynamic(this, &UEXOptionsGeneral::HitMarkersChanged);

	HoldObjective->SetCheckedState(Settings->GetHoldObjective() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	HoldObjective->OnCheckStateChanged.AddDynamic(this, &UEXOptionsGeneral::HoldObjectiveChanged);

	ToggleSprint->SetCheckedState(Settings->GetToggleSprint() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	ToggleSprint->OnCheckStateChanged.AddDynamic(this, &UEXOptionsGeneral::ToggleSprintChanged);

	ToggleWalk->SetCheckedState(Settings->GetToggleWalk() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	ToggleWalk->OnCheckStateChanged.AddDynamic(this, &UEXOptionsGeneral::ToggleWalkChanged);

	ToggleADS->SetCheckedState(Settings->GetToggleADS() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	ToggleADS->OnCheckStateChanged.AddDynamic(this, &UEXOptionsGeneral::ToggleADSChanged);

	ADSCrosshair->SetCheckedState(Settings->GetADSCrosshair() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	ADSCrosshair->OnCheckStateChanged.AddDynamic(this, &UEXOptionsGeneral::ADSCrosshairChanged);

	ReloadCancelsReload->SetCheckedState(Settings->GetReloadCancelsReload() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	ReloadCancelsReload->OnCheckStateChanged.AddDynamic(this, &UEXOptionsGeneral::ReloadCancelsReloadChanged);
}

void UEXOptionsGeneral::HitMarkersChanged(bool bIsChecked)
{
	Settings->SetHitMarkers(bIsChecked);
}

void UEXOptionsGeneral::HoldObjectiveChanged(bool bIsChecked)
{
	Settings->SetHoldObjective(bIsChecked);
}

void UEXOptionsGeneral::ToggleSprintChanged(bool bIsChecked)
{
	Settings->SetToggleSprint(bIsChecked);
}

void UEXOptionsGeneral::ToggleWalkChanged(bool bIsChecked)
{
	Settings->SetToggleWalk(bIsChecked);
}

void UEXOptionsGeneral::ToggleADSChanged(bool bIsChecked)
{
	Settings->SetToggleADS(bIsChecked);
}

void UEXOptionsGeneral::ADSCrosshairChanged(bool bIsChecked)
{
	Settings->SetADSCrosshair(bIsChecked);
}

void UEXOptionsGeneral::ReloadCancelsReloadChanged(bool bIsChecked)
{
	Settings->SetReloadCancelsReload(bIsChecked);
}
