// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Math/Color.h"
#include "EXPersistentUser.generated.h"

struct FInputBind;
class APlayerController;

USTRUCT(BlueprintType)
struct FMyInputId
{
	GENERATED_BODY()

	UPROPERTY()
	FName Name;
	UPROPERTY()
	bool bInverted = false;

	FMyInputId() : Name(""), bInverted(false) { }
	FMyInputId(FName InName, bool bInInverted = false) : Name(InName), bInverted(bInInverted) { }

	friend bool operator==(const FMyInputId& A, const FMyInputId& B)
	{
		return (A.Name.IsEqual(B.Name) && (A.bInverted == B.bInverted));
	}

	/** Needed for TMap::GetTypeHash() */
	friend uint32 GetTypeHash(const FMyInputId& A)
	{
		FName UniqueName(A.Name, A.bInverted ? 1 : 0);
		return GetTypeHash(UniqueName);
	}
};

USTRUCT()
struct FCrosshair
{
	GENERATED_BODY()

	// CP = Plus, CC = Circle, CD = Dot, CX = All
	UPROPERTY()
	float CP_Gap = 4.f;
	UPROPERTY()
	float CP_Size = 10.f;
	UPROPERTY()
	float CP_Thickness = 2.f;
	UPROPERTY()
	float CP_DynamicMultiplier = 300.f;
	UPROPERTY()
	float CD_Size = 0.f;
	UPROPERTY()
	float CC_Size = 0.f;
	UPROPERTY()
	float CC_Thickness = 1.f;
	UPROPERTY()
	float CC_DynamicMultiplier = 300.f;
	UPROPERTY()
	FLinearColor CX_Color = FLinearColor::Green;

	friend bool operator==(const FCrosshair& LHS, const FCrosshair& RHS)
	{
		bool bEqual = true;
		bEqual &= LHS.CP_Gap == RHS.CP_Gap;
		bEqual &= LHS.CP_Size == RHS.CP_Size;
		bEqual &= LHS.CP_Thickness == RHS.CP_Thickness;
		bEqual &= LHS.CP_DynamicMultiplier == RHS.CP_DynamicMultiplier;
		bEqual &= LHS.CD_Size == RHS.CD_Size;
		bEqual &= LHS.CC_Size == RHS.CC_Size;
		bEqual &= LHS.CC_Thickness == RHS.CC_Thickness;
		bEqual &= LHS.CC_DynamicMultiplier == RHS.CC_DynamicMultiplier;
		bEqual &= LHS.CX_Color == RHS.CX_Color;
		return bEqual;
	}

	friend bool operator!=(const FCrosshair& LHS, const FCrosshair& RHS)
	{
		return !(LHS == RHS);
	}
};

/**
 * Class for storing user settings
 */
UCLASS()
class EX_API UEXPersistentUser : public USaveGame
{
	GENERATED_BODY()
	
public:
	UEXPersistentUser();

	static UEXPersistentUser* LoadPersistentUser(FString SlotName, const int32 UserIndex);

	/** Saves data if anything has changed. */
	void SaveIfDirty();

	/** Needed because we can recreate the subsystem that stores it */
	void TellInputAboutKeybindings();

	void UpdateInputBindingInfo(APlayerController* PC);

	FORCEINLINE int32 GetUserIndex() const { return UserIndex; }


	/** Is the y axis inverted? */
	FORCEINLINE bool GetInvertedYAxis() const
	{
		return bInvertedYAxis;
	}
	/** Setter for inverted y axis */
	void SetInvertedYAxis(bool bInvert);

	/** Getter for the aim sensitivity */
	FORCEINLINE float GetAimSensitivity() const
	{
		return AimSensitivity;
	}
	void SetAimSensitivity(float InSensitivity);

	/** Getter for the scope sensitivity scale */
	FORCEINLINE float GetScopeSensitivityScale() const
	{
		return ScopeSensitivityScale;
	}
	void SetScopeSensitivityScale(float InScopeSensitivityScale);

	/** Getter for the gamma correction */
	FORCEINLINE float GetGamma() const
	{
		return Gamma;
	}
	void SetGamma(float InGamma);

	void SetChangedInputs(TMap<FMyInputId, FKey> InChangedInputs);

	/** Getter for the master volume */
	FORCEINLINE float GetMasterVolume() const
	{
		return MasterVolume;
	}
	void SetMasterVolume(float InMasterVolume);
	FORCEINLINE bool GetMuted() const
	{
		return bMuted;
	}
	void SetMuted(bool bInMuted);
	/** Getter for the music volume */
	FORCEINLINE float GetMusicVolume() const
	{
		return MusicVolume;
	}
	void SetMusicVolume(float InMusicVolume);

	/** Getter for the default FOV */
	FORCEINLINE float GetFOV() const
	{
		return FOV;
	}
	void SetFOV(float InFOV);

	/** Getter for whether the player needs to hold the key to interact with an objective or toggle */
	FORCEINLINE bool GetHoldObjective() const
	{
		return bHoldObjective;
	}
	void SetHoldObjective(bool bInHoldObjective);

	/** Getter for whether hit markers are displayed on HUD */
	FORCEINLINE bool GetHitMarkers() const
	{
		return bHitMarkers;
	}
	void SetHitMarkers(bool bInHitMarkers);

	/** Getter for whether sprint is toggled. */
	FORCEINLINE bool GetToggleSprint() const
	{
		return bToggleSprint;
	}
	void SetToggleSprint(bool bInToggleSprint);

	/** Getter for whether walk is toggled. */
	FORCEINLINE bool GetToggleWalk() const
	{
		return bToggleWalk;
	}
	void SetToggleWalk(bool bInToggleWalk);

	/** Getter for whether ADS is toggled. */
	FORCEINLINE bool GetToggleADS() const
	{
		return bToggleADS;
	}
	void SetToggleADS(bool bInToggleADS);

	/** Getter for whether crosshair is shown when ADSing. */
	FORCEINLINE bool GetADSCrosshair() const
	{
		return bADSCrosshair;
	}
	void SetADSCrosshair(bool bInADSCrosshair);

	/** Getter for whether crosshair is shown when ADSing. */
	FORCEINLINE bool GetReloadCancelsReload() const
	{
		return bReloadCancelsReload;
	}
	void SetReloadCancelsReload(bool bInReloadCancelsReload);

	FORCEINLINE TArray<FCrosshair> GetCrosshairs() const
	{
		return Crosshairs;
	}
	void SetCrosshairs(const TArray<FCrosshair>& InCrosshairs);

	FORCEINLINE uint32 GetActiveCrosshairIdx() const
	{
		return ActiveCrosshairIdx;
	}
	void SetActiveCrosshairIdx(uint32 InActiveCrosshairIdx);

protected:

	void SetToDefaults();

	void SavePersistenUser();


	UPROPERTY()
	float AimSensitivity;
	UPROPERTY()
	float ScopeSensitivityScale;

	/** Holds the gamma correction setting */
	UPROPERTY()
	float Gamma;
	
	UPROPERTY()
	float MasterVolume;
	UPROPERTY()
	bool bMuted;
	UPROPERTY()
	float MusicVolume;
	
	UPROPERTY()
	float FOV;

	UPROPERTY()
	bool bInvertedYAxis;

	UPROPERTY(Transient)
	TMap<FMyInputId, FKey> ChangedInputs;

	UPROPERTY()
	bool bHitMarkers;
	UPROPERTY()
	bool bHoldObjective;
	UPROPERTY()
	bool bToggleSprint;
	UPROPERTY()
	bool bToggleWalk;
	UPROPERTY()
	bool bToggleADS;
	UPROPERTY()
	bool bADSCrosshair;
	UPROPERTY()
	bool bReloadCancelsReload;

	UPROPERTY()
	TArray<FCrosshair> Crosshairs;
	UPROPERTY()
	uint32 ActiveCrosshairIdx = 0;
	/// 

private:
	/** Internal.  True if data is changed but hasn't been saved. */
	bool bIsDirty;

	/** The string identifier used to save/load this persistent user. */
	FString SlotName;
	int32 UserIndex;
};
