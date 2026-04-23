// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Player/EXPersistentUser.h"

class UEXPersistentUser;
class ULocalPlayer;
class UEXGameUserSettings;
class UEXLocalPlayer;

/** Supported resolutions */
const FIntPoint DefaultShooterResolutions[] = { 
	FIntPoint(1024,768), 
	FIntPoint(1280,720),
	FIntPoint(1920,1080),
	FIntPoint(3440,1440),
};

/** Supported resolutions count*/
const int32 DefaultShooterResCount = UE_ARRAY_COUNT(DefaultShooterResolutions);

/** Delegate called when changes are applied */
DECLARE_DELEGATE(FOnApplyChanges);


class FEXSettings : public TSharedFromThis<FEXSettings>
{
public:
	/** Sets owning player controller */
	void Construct(UEXLocalPlayer* InPlayerOwner);

	/** Get current options values for display */
	void UpdateOptions();

	/** Applies changes in game settings */
	void ApplySettings();

	/** Needed because we can recreate the subsystem that stores it */
	void TellInputAboutKeybindings();

	/** Reverts non-saved changes in game settings */
	void RevertChanges();


	/** Get current user index out of PlayerOwner */
	int32 GetOwnerUserIndex() const;

	/** Get the persistence user associated with PlayerOwner*/
	UEXPersistentUser* GetPersistentUser() const;

	/** Called when changes were applied - can be used to close submenu */
	FOnApplyChanges OnApplyChanges;

protected:

	/** Current gamma correction set in options */
	float GammaOpt = 2.2f;

	/** Full screen setting set in options */
	EWindowMode::Type bFullScreenOpt = EWindowMode::Fullscreen;

	/** Graphics quality option */
	int32 GraphicsQualityOpt;

	float MaxFrameRate;

	bool bVSyncOpt;

	/** Resolution setting set in options */
	FIntPoint ResolutionOpt;

	/** Available resolutions list */
	TArray<FIntPoint> Resolutions;

	/** Owning player controller */
	UEXLocalPlayer* PlayerOwner = nullptr;

	/** User settings pointer */
	UEXGameUserSettings* UserSettings;

	/** Vertical FOV */
	float FOV;

	/** Multiplier for all sounds in game */
	float MasterVolume;

	/** Multiplier for music volume */
	float MusicVolume;

	/** Mouse sens */
	float AimSensitivity;

	/** Current scope sensitivity scale set in options */
	float ScopeSensitivityScale;

	/** Invert mouse setting set in options */
	uint8 bInvertYAxisOpt : 1;

	/** Whether we should hold the interact button on objectives or toggle */
	bool bHoldObjective;

	/** Press sprint once to start sprinting and twice to stop */
	bool bToggleSprint;

	/** Press walk once to start sprinting and twice to stop */
	bool bToggleWalk;

	/** Press sprint once to start ADSing and twice to stop */
	bool bToggleADS;

	/** Show crosshair when ADSing? */
	bool bADSCrosshair;

	/** Cancel reload when pressing the reload button if already reloading? */
	bool bReloadCancelsReload;

	/** The keys that are bind to a different key than the defaults */
	TMap<FMyInputId, FKey> ChangedInputs;

	/** Whether hit markers should be displayed on HUD */
	bool bHitMarkers;

	/** Active crosshair */
	TArray<FCrosshair> Crosshairs;
	uint32 ActiveCrosshairIdx;
	///

public:

	FORCEINLINE bool GetInvertYAxis() const { return bInvertYAxisOpt; }
	void SetInvertYAxis(float bInInvertYAxisOpt) { bInvertYAxisOpt = bInInvertYAxisOpt; }

	FORCEINLINE TArray<FIntPoint> GetResolutions() const { return Resolutions; }
	FORCEINLINE float GetMaxFrameRate() const { return MaxFrameRate; }
	void SetMaxFrameRate(float InMaxFrameRate) { MaxFrameRate = InMaxFrameRate; }
	FORCEINLINE bool GetVSync() const { return bVSyncOpt; }
	void SetVSync(bool bUseVSync) { bVSyncOpt = bUseVSync; }
	FORCEINLINE int32 GetGraphicsQualityOpt() const { return GraphicsQualityOpt; }
	void SetGraphicsQualityOpt(int32 InGraphicsQualityOpt) { GraphicsQualityOpt = InGraphicsQualityOpt; }
	FORCEINLINE FIntPoint GetResolutionOpt() const { return ResolutionOpt; }
	void SetResolutionOpt(int32 Opt) { ResolutionOpt = Resolutions[Opt]; }
	FORCEINLINE EWindowMode::Type GetFullScreen() const { return bFullScreenOpt; }
	void SetFullscreen(EWindowMode::Type bInWindowMode) { bFullScreenOpt = bInWindowMode; }

	FORCEINLINE float GetFOV() const { return FOV; }
	void SetFOV(float InFOV) { FOV = InFOV; }

	FORCEINLINE float GetMasterVolume() const { return MasterVolume; }
	void SetMasterVolume(float InMasterVolume) { MasterVolume = InMasterVolume; }
	FORCEINLINE float GetMusicVolume() const { return MusicVolume; }
	void SetMusicVolume(float InMusicVolume) { MusicVolume = InMusicVolume; }

	FORCEINLINE float GetAimSensitivity() const { return AimSensitivity; }
	void SetAimSensitivity(float InAimSensitivity) { AimSensitivity = InAimSensitivity; }
	FORCEINLINE float GetScopeSensitivityScale() const { return ScopeSensitivityScale; }
	void SetScopeSensitivityScale(float InScopeSensitivityScale) { ScopeSensitivityScale = InScopeSensitivityScale; }

	FORCEINLINE bool GetHitMarkers() const { return bHitMarkers; }
	void SetHitMarkers(bool bInHitMarkers) { bHitMarkers = bInHitMarkers; }
	FORCEINLINE bool GetHoldObjective() const { return bHoldObjective; }
	void SetHoldObjective(bool bInHoldObjective) { bHoldObjective = bInHoldObjective; }
	FORCEINLINE bool GetToggleSprint() const { return bToggleSprint; }
	void SetToggleSprint(bool bInToggleSprint) { bToggleSprint = bInToggleSprint; }
	FORCEINLINE bool GetToggleWalk() const { return bToggleWalk; }
	void SetToggleWalk(bool bInToggleWalk) { bToggleWalk = bInToggleWalk; }
	FORCEINLINE bool GetToggleADS() const { return bToggleADS; }
	void SetToggleADS(bool bInToggleADS) { bToggleADS = bInToggleADS; }
	FORCEINLINE bool GetADSCrosshair() const { return bADSCrosshair; }
	void SetADSCrosshair(bool bInADSCrosshair) { bADSCrosshair = bInADSCrosshair; }
	FORCEINLINE bool GetReloadCancelsReload() const { return bReloadCancelsReload; }
	void SetReloadCancelsReload(bool bInReloadCancelsReload) { bReloadCancelsReload = bInReloadCancelsReload; }

	FORCEINLINE TArray<FCrosshair> GetCrosshairs() const { return Crosshairs; }
	void SetCrosshairs(const TArray<FCrosshair>& InCrosshair) { Crosshairs = InCrosshair; }
	FORCEINLINE uint32 GetActiveCrosshairIdx() const { return ActiveCrosshairIdx; }
	void SetActiveCrosshairIdx(uint32 InActiveCrosshairIdx) { ActiveCrosshairIdx = InActiveCrosshairIdx; }


	void ChangeKey(const FMyInputId& Id, const FKey& Key) 
	{
		ChangedInputs.FindOrAdd(Id) = Key;
	};

};
