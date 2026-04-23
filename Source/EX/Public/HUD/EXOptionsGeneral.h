// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXOptionsGeneral.generated.h"

class FEXSettings;
class UCheckBox;

/**
 * 
 */
UCLASS()
class EX_API UEXOptionsGeneral : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(TSharedPtr<FEXSettings> InSettings, bool bReset);

protected:
	UFUNCTION()
	void HitMarkersChanged(bool bIsChecked);
	UFUNCTION()
	void HoldObjectiveChanged(bool bIsChecked);
	UFUNCTION()
	void ToggleSprintChanged(bool bIsChecked);
	UFUNCTION()
	void ToggleWalkChanged(bool bIsChecked);
	UFUNCTION()
	void ToggleADSChanged(bool bIsChecked);
	UFUNCTION()
	void ADSCrosshairChanged(bool bIsChecked);
	UFUNCTION()
	void ReloadCancelsReloadChanged(bool bIsChecked);

	UPROPERTY(Meta = (BindWidget))
	UCheckBox* HitMarkers;
	UPROPERTY(Meta = (BindWidget))
	UCheckBox* HoldObjective;
	UPROPERTY(Meta = (BindWidget))
	UCheckBox* ToggleSprint;
	UPROPERTY(Meta = (BindWidget))
	UCheckBox* ToggleWalk;
	UPROPERTY(Meta = (BindWidget))
	UCheckBox* ToggleADS;
	UPROPERTY(Meta = (BindWidget))
	UCheckBox* ADSCrosshair;
	UPROPERTY(Meta = (BindWidget))
	UCheckBox* ReloadCancelsReload;

	TSharedPtr<FEXSettings> Settings;
};
