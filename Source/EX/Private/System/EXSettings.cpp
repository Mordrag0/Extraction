// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EXSettings.h"
#include "Player/EXPersistentUser.h"
#include "Player/EXLocalPlayer.h"
#include "System/EXGameUserSettings.h"
#include "System/EXGameplayStatics.h"


void FEXSettings::Construct(UEXLocalPlayer* InPlayerOwner)
{
	PlayerOwner = InPlayerOwner;

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetInitialDisplayMetrics(DisplayMetrics);

	bool bAddedNativeResolution = false;
	const FIntPoint NativeResolution(DisplayMetrics.PrimaryDisplayWidth, DisplayMetrics.PrimaryDisplayHeight);

	//TArray<FText> ResolutionList;
	for (const auto& DefaultShooterResolution : DefaultShooterResolutions)
	{
		if (DefaultShooterResolution.X <= DisplayMetrics.PrimaryDisplayWidth && DefaultShooterResolution.Y <= DisplayMetrics.PrimaryDisplayHeight)
		{
			//ResolutionList.Add(FText::Format(FText::FromString("{0}x{1}"), FText::FromString(FString::FromInt(DefaultShooterResolution.X)), FText::FromString(FString::FromInt(DefaultShooterResolution.Y))));
			Resolutions.Add(DefaultShooterResolution);

			bAddedNativeResolution |= (DefaultShooterResolution == NativeResolution);
		}
	}

	// Always make sure that the native resolution is available
	if (!bAddedNativeResolution)
	{
		//ResolutionList.Add(FText::Format(FText::FromString("{0}x{1}"), FText::FromString(FString::FromInt(NativeResolution.X)), FText::FromString(FString::FromInt(NativeResolution.Y))));
		Resolutions.Add(NativeResolution);
	}


	UserSettings = CastChecked<UEXGameUserSettings>(GEngine->GetGameUserSettings());
	MaxFrameRate = GetMaxFrameRate();
	bVSyncOpt = UserSettings->IsVSyncEnabled();
	ResolutionOpt = UserSettings->GetScreenResolution();
	bFullScreenOpt = UserSettings->GetFullscreenMode();
	GraphicsQualityOpt = UserSettings->GetGraphicsQuality();

	UpdateOptions();
}

void FEXSettings::UpdateOptions()
{
	UEXPersistentUser* const PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		// Update bInvertYAxisOpt, SensitivityOpt and GammaOpt because the EXOptions can be created without the controller having a player
		// by the in-game menu which will leave them with default values
		GammaOpt = PersistentUser->GetGamma();

		FOV = PersistentUser->GetFOV();

		MasterVolume = PersistentUser->GetMasterVolume();
		MusicVolume = PersistentUser->GetMusicVolume();

		AimSensitivity = PersistentUser->GetAimSensitivity();
		ScopeSensitivityScale = PersistentUser->GetScopeSensitivityScale();
		bInvertYAxisOpt = PersistentUser->GetInvertedYAxis();
		ChangedInputs.Empty();

		bHitMarkers = PersistentUser->GetHitMarkers();
		bHoldObjective = PersistentUser->GetHoldObjective();
		bToggleSprint = PersistentUser->GetToggleSprint();
		bToggleWalk = PersistentUser->GetToggleWalk();
		bToggleADS = PersistentUser->GetToggleADS();
		bADSCrosshair = PersistentUser->GetADSCrosshair();
		bReloadCancelsReload = PersistentUser->GetReloadCancelsReload();

		Crosshairs = PersistentUser->GetCrosshairs();
		ActiveCrosshairIdx = PersistentUser->GetActiveCrosshairIdx();
	}

	/*InvertYAxisOption->SelectedMultiChoice = GetCurrentMouseYAxisInvertedIndex();
	AimSensitivityOption->SelectedMultiChoice = GetCurrentMouseSensitivityIndex();
	GammaOption->SelectedMultiChoice = GetCurrentGammaIndex();
	VibrationOption->SelectedMultiChoice = bVibrationOpt ? 1 : 0;

	GammaOptionChanged(GammaOption, GammaOption->SelectedMultiChoice);
#if PLATFORM_DESKTOP
	VideoResolutionOption->SelectedMultiChoice = GetCurrentResolutionIndex(UserSettings->GetScreenResolution());
	GraphicsQualityOption->SelectedMultiChoice = UserSettings->GetGraphicsQuality();
	FullScreenOption->SelectedMultiChoice = UserSettings->GetFullscreenMode() != EWindowMode::Windowed ? 1 : 0;
#endif*/
}

void FEXSettings::ApplySettings()
{
	UEXPersistentUser* PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->SetGamma(GammaOpt);

		PersistentUser->SetFOV(FOV);

		PersistentUser->SetMasterVolume(MasterVolume);
		PersistentUser->SetMusicVolume(MusicVolume);

		PersistentUser->SetAimSensitivity(AimSensitivity);
		PersistentUser->SetScopeSensitivityScale(ScopeSensitivityScale);
		PersistentUser->SetInvertedYAxis(bInvertYAxisOpt);
		PersistentUser->SetChangedInputs(ChangedInputs);

		PersistentUser->SetHitMarkers(bHitMarkers);
		PersistentUser->SetHoldObjective(bHoldObjective);
		PersistentUser->SetToggleSprint(bToggleSprint);
		PersistentUser->SetToggleWalk(bToggleWalk);
		PersistentUser->SetToggleADS(bToggleADS);
		PersistentUser->SetADSCrosshair(bADSCrosshair);
		PersistentUser->SetReloadCancelsReload(bReloadCancelsReload);

		PersistentUser->SetCrosshairs(Crosshairs);
		PersistentUser->SetActiveCrosshairIdx(ActiveCrosshairIdx);

		PersistentUser->SaveIfDirty();
		PersistentUser->TellInputAboutKeybindings();
	}
#if !WITH_EDITOR
	UserSettings->SetFrameRateLimit(MaxFrameRate);
	UserSettings->SetVSyncEnabled(bVSyncOpt);
	UserSettings->SetScreenResolution(ResolutionOpt);
	UserSettings->SetFullscreenMode(bFullScreenOpt);
	UserSettings->SetGraphicsQuality(GraphicsQualityOpt);
	UserSettings->ApplySettings(false);
	UE_LOG(LogTemp, Warning, TEXT("Settings applied"));
#endif

	OnApplyChanges.ExecuteIfBound();
}

void FEXSettings::TellInputAboutKeybindings()
{
	UEXPersistentUser* PersistentUser = GetPersistentUser();
	if (PersistentUser)
	{
		PersistentUser->TellInputAboutKeybindings();
	}
}

void FEXSettings::RevertChanges()
{
	UpdateOptions();
}

int32 FEXSettings::GetOwnerUserIndex() const
{
	return PlayerOwner ? PlayerOwner->GetControllerId() : 0;
}

UEXPersistentUser* FEXSettings::GetPersistentUser() const
{
	if (PlayerOwner)
	{
		return PlayerOwner->GetPersistentUser();
	}
	return nullptr;
}
