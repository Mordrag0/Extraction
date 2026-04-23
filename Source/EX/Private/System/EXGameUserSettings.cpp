// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EXGameUserSettings.h"

void UEXGameUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	ScalabilityQuality.SetFromSingleQualityLevel(GraphicsQuality);

	Super::ApplySettings(bCheckForCommandLineOverrides);
}

void UEXGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	GraphicsQuality = 1;
	bIsDedicatedServer = false;
}
