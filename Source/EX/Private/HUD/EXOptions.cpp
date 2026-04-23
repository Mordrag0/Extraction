// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptions.h"
#include "HUD/EXOptionsInput.h"
#include "Player/EXLocalPlayer.h"
#include "HUD/EXOptionsVideo.h"
#include "System/EXSettings.h"
#include "HUD/EXOptionsAudio.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Online/EXMenuController.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/EXOptionsCrosshair.h"
#include "System/EXGameplayStatics.h"
#include "Misc/EXMusicManager.h"
#include "HUD/EXOptionsGeneral.h"
#include "HUD/EXOptionsHUD.h"

void UEXOptions::Init(UEXLocalPlayer* LP)
{
	Settings = MakeShareable(new FEXSettings());
	Settings->Construct(LP);
	Settings->TellInputAboutKeybindings();
	InputOptions->Init(Settings, false);
	VideoOptions->Init(Settings, false);
	AudioOptions->Init(Settings, false);
	CrosshairOptions->Init(Settings, false);
	GeneralOptions->Init(Settings, false);
	HUDOptions->Init(Settings, false);

	SaveButton->OnClicked.AddDynamic(this, &UEXOptions::Save);
	ResetButton->OnClicked.AddDynamic(this, &UEXOptions::Reset);
	CloseButton->OnClicked.AddDynamic(this, &UEXOptions::Close);
}

void UEXOptions::Save()
{
#if !WITH_EDITOR
	if (VideoOptions->GetScreenSettingsDirty())
	{
		VideoOptions->ApplySettings();
	}
#endif
	Settings->SetCrosshairs(CrosshairOptions->GetCrosshairs());
	Settings->SetActiveCrosshairIdx(CrosshairOptions->GetActiveCrosshairIdx());
	Settings->ApplySettings();

	if (AudioOptions->IsDirty()) // If audio options are dirty, we need to restart music
	{
		AudioOptions->ClearDirty();
		
		UEXGameplayStatics::SetMasterVolume(Settings->GetPersistentUser()->GetMasterVolume());
		UEXGameplayStatics::SetMusicVolume(Settings->GetPersistentUser()->GetMusicVolume());
		AEXMusicManager* MusicManager = UEXGameplayStatics::GetMusicManager(this);
		if (MusicManager)
		{
			MusicManager->RefreshVolume();
		}
	}
}

void UEXOptions::Reset()
{
	Settings->RevertChanges();
	InputOptions->Init(Settings, true);
	VideoOptions->Init(Settings, true);
	AudioOptions->Init(Settings, true);
	CrosshairOptions->Init(Settings, true);
	GeneralOptions->Init(Settings, true);
	HUDOptions->Init(Settings, true);
}

void UEXOptions::Close()
{
	OnClosed.ExecuteIfBound();
}



