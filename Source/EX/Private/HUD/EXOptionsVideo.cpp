// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptionsVideo.h"
#include "Player/EXLocalPlayer.h"
#include "System/EXSettings.h"
#include "Components/ComboBoxString.h"
#include "Player/EXPersistentUser.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"

void UEXOptionsVideo::Init(TSharedPtr<FEXSettings> InSettings, bool bReset)
{
	Settings = InSettings;
	TArray<FIntPoint> Resolutions = Settings->GetResolutions();

	for (const FIntPoint& Resolution : Resolutions)
	{
		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(FStringFormatArg(FString::FromInt(Resolution.X)));
		StringArgs.Add(FStringFormatArg(FString::FromInt(Resolution.Y)));
		FString ResolutionText = FString::Format(TEXT("{0}x{1}"), StringArgs);
		ResolutionOptions->AddOption(ResolutionText);
		if (Resolution == Settings->GetResolutionOpt())
		{
			ResolutionOptions->SetSelectedIndex(Resolutions.IndexOfByKey(Resolution));
		}
	}
	if (ResolutionOptions->GetSelectedIndex() == -1)
	{
		ResolutionOptions->SetSelectedIndex(0);
	}
	CurrentResolutionOpt = ResolutionOptions->GetSelectedIndex();
	if (!bReset)
	{
		ResolutionOptions->OnSelectionChanged.AddDynamic(this, &UEXOptionsVideo::OnResolutionChanged);
	}
	EWindowMode::Type CurrentWindowType = Settings->GetFullScreen();
	for (int32 WindowTypeIdx = 0; WindowTypeIdx < 3; WindowTypeIdx++)
	{
		const EWindowMode::Type& WindowType = static_cast<EWindowMode::Type>(WindowTypeIdx);
		FString WindowTypeText(LexToString(WindowType));
		WindowOptions->AddOption(WindowTypeText);
		if (CurrentWindowType == WindowType)
		{
			WindowOptions->SetSelectedIndex(WindowTypeIdx);
		}
	}
	if (WindowOptions->GetSelectedIndex() == -1)
	{
		WindowOptions->SetSelectedIndex(0);
	}
	CurrentWindowOpt = WindowOptions->GetSelectedIndex();
	if (!bReset)
	{
		WindowOptions->OnSelectionChanged.AddDynamic(this, &UEXOptionsVideo::OnWindowOptionChanged);
	}

	float FOV = Settings->GetFOV();
	FOVSlider->SetValue((FOV - MinFOV) / (MaxFOV - MinFOV));
	FOVText->SetText(FText::FromString(FString::SanitizeFloat(FOV)));

	if (!bReset)
	{
		FOVSlider->OnValueChanged.AddDynamic(this, &UEXOptionsVideo::FOVChanged);
	}

	CurrentQualityOpt = Settings->GetGraphicsQualityOpt();
	ComboQuality->SetSelectedIndex(CurrentQualityOpt);
	if (!bReset)
	{
		ComboQuality->OnSelectionChanged.AddDynamic(this, &UEXOptionsVideo::QualityChanged);
	}

	CurrentVSyncValue = Settings->GetVSync();
	CheckVSync->SetCheckedState(CurrentVSyncValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	if (!bReset)
	{
		CheckVSync->OnCheckStateChanged.AddDynamic(this, &UEXOptionsVideo::VSyncChanged);
		VideoOptionsApply->OnClicked.AddDynamic(this, &UEXOptionsVideo::ApplySettings);
	}
}

void UEXOptionsVideo::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	bScreenSettingsDirty |= ResolutionOptions->GetSelectedIndex() != CurrentResolutionOpt;
}

void UEXOptionsVideo::OnWindowOptionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	bScreenSettingsDirty |= WindowOptions->GetSelectedIndex() != CurrentWindowOpt;
}

void UEXOptionsVideo::QualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	bScreenSettingsDirty |= CurrentQualityOpt != ComboQuality->GetSelectedIndex();
}

void UEXOptionsVideo::VSyncChanged(bool bIsChecked)
{
	bScreenSettingsDirty |= bIsChecked == CurrentVSyncValue;
}

TArray<FIntPoint> UEXOptionsVideo::GetResolutionOptions() const
{
	return Settings->GetResolutions();
}

void UEXOptionsVideo::ApplySettings()
{
	Settings->SetMaxFrameRate(0.f);
	Settings->SetVSync(CheckVSync->IsChecked());
	Settings->SetGraphicsQualityOpt(ComboQuality->GetSelectedIndex());
	Settings->SetResolutionOpt(ResolutionOptions->GetSelectedIndex());
	Settings->SetFullscreen(EWindowMode::ConvertIntToWindowMode(WindowOptions->GetSelectedIndex()));
	Settings->ApplySettings();
	bScreenSettingsDirty = false;
}

void UEXOptionsVideo::FOVChanged(float Val)
{
	const float FOV = FMath::Clamp(MinFOV + (MaxFOV - MinFOV) * Val, MinFOV, MaxFOV); ;
	FOVText->SetText(FText::FromString(FString::SanitizeFloat(FOV)));
	Settings->SetFOV(FOV);
}

