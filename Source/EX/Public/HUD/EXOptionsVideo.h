// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXOptionsVideo.generated.h"

class UEXLocalPlayer;
class FEXSettings;
class UButton;

/**
 * 
 */
UCLASS()
class EX_API UEXOptionsVideo : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void Init(TSharedPtr<FEXSettings> InSettings, bool bReset);

	bool GetScreenSettingsDirty() const { return bScreenSettingsDirty; }

	UFUNCTION()
	void ApplySettings();

protected:

	UFUNCTION()
	void QualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	int32 CurrentQualityOpt = 0;

	UFUNCTION()
	void VSyncChanged(bool bIsChecked);
	bool CurrentVSyncValue = false;

	UFUNCTION(BlueprintCallable, Category = "Video Options")
	TArray<FIntPoint> GetResolutionOptions() const;
	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	int32 CurrentResolutionOpt = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Video Options", Meta = (BindWidget))
	class UComboBoxString* ResolutionOptions = nullptr;

	UFUNCTION()
	void OnWindowOptionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	int32 CurrentWindowOpt = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Video Options", Meta = (BindWidget))
	class UComboBoxString* WindowOptions = nullptr;

	UFUNCTION()
	void FOVChanged(float Val);

	TSharedPtr<FEXSettings> Settings;

	UPROPERTY(Meta = (BindWidget))
	UButton* VideoOptionsApply = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Video Options")
	float MinFOV = 42.f;
	UPROPERTY(EditDefaultsOnly, Category = "Video Options")
	float MaxFOV = 90.f;

	UPROPERTY(Meta = (BindWidget))
	class USlider* FOVSlider = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* FOVText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* CheckVSync = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class UComboBoxString* ComboQuality = nullptr;

	bool bScreenSettingsDirty = false;
};
