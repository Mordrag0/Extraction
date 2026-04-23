// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptionsAudio.h"
#include "Player/EXPersistentUser.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "System/EXSettings.h"

void UEXOptionsAudio::Init(TSharedPtr<FEXSettings> InSettings, bool bReset)
{
	Settings = InSettings;

	MasterVolume = Settings->GetMasterVolume();
	MasterVolumeText->SetText(FText::FromString(FString::SanitizeFloat(MasterVolume)));
	MasterSlider->SetValue(MasterVolume);

	MusicVolume = Settings->GetMusicVolume();
	MusicVolumeText->SetText(FText::FromString(FString::SanitizeFloat(MusicVolume)));
	MusicSlider->SetValue(MusicVolume);

	if (!bReset)
	{
		MasterSlider->OnValueChanged.AddDynamic(this, &UEXOptionsAudio::MasterValueChanged);
		MusicSlider->OnValueChanged.AddDynamic(this, &UEXOptionsAudio::MusicValueChanged);
	}
	bDirty = false;
}

void UEXOptionsAudio::MasterValueChanged(float Value)
{
	MasterVolume = Value;
	MasterVolumeText->SetText(FText::FromString(FString::SanitizeFloat(MasterVolume)));
	Settings->SetMasterVolume(MasterVolume);
	bDirty = true;
}

void UEXOptionsAudio::MusicValueChanged(float Value)
{
	MusicVolume = Value;
	MusicVolumeText->SetText(FText::FromString(FString::SanitizeFloat(MusicVolume)));
	Settings->SetMusicVolume(MusicVolume);
	bDirty = true;
}
