// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptionsInput.h"
#include "Player/EXPersistentUser.h"
#include "Player/EXLocalPlayer.h"
#include "HUD/EXOptionsInputRow.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/UniformGridPanel.h"
#include "Components/CheckBox.h"
#include "Components/UniformGridSlot.h"
#include "System/EXSettings.h"
#include "Misc/EXInputInfo.h"
#include "System/EXGameplayStatics.h"

void UEXOptionsInput::Init(TSharedPtr<FEXSettings> InSettings, bool bReset)
{
	Settings = InSettings;
	UEXInputInfo* InputInfo = UEXGameplayStatics::GetInputInfoCDO();
	const TMap<FMyInputId, FInputBindValue>& Inputs = InputInfo->Inputs;

	// Keys
	if ((Rows.Num() == 0) && RowsPanel)
	{
		RowsPanel->ClearChildren();
		Rows.Empty();
		int32 RowIdx = 0;
		for (const TPair<FMyInputId, FInputBindValue>& Input : Inputs)
		{
			UEXOptionsInputRow* Row = CreateWidget<UEXOptionsInputRow>(this, RowClass);
			UUniformGridSlot* GridSlot = RowsPanel->AddChildToUniformGrid(Row, RowIdx++);
			GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			Row->Init(this);
			Row->Fill(Input.Key.Name, Input.Value.DisplayName, Input.Key.bInverted, Input.Value.Key);
			Rows.Add(Row);
		}
	}

	// Mouse
	bInvertYAxis = Settings->GetInvertYAxis();
	InvertYAxis->SetIsChecked(bInvertYAxis);
	MouseSens = Settings->GetAimSensitivity();

	MouseSensValue->SetValue(SensToSliderValue(MouseSens));
	MouseSensValueText->SetText(FText::FromString(FString::SanitizeFloat(MouseSens)));

	ScopeSensitivityScale = Settings->GetScopeSensitivityScale();
	ScopeSensScaleValue->SetValue(ScopeSensitivityScale);
	ScopeSensScaleValueText->SetText(FText::FromString(FString::SanitizeFloat(ScopeSensitivityScale)));
	if (!bReset)
	{
		InvertYAxis->OnCheckStateChanged.AddDynamic(this, &UEXOptionsInput::InvertYAxisChanged);
		MouseSensValue->OnValueChanged.AddDynamic(this, &UEXOptionsInput::MouseSensChanged);
		ScopeSensScaleValue->OnValueChanged.AddDynamic(this, &UEXOptionsInput::ScopeSensScaleChanged);
	}
}

void UEXOptionsInput::InputChanged(FMyInputId Id, FKey Key)
{
	Settings->ChangeKey(Id, Key);
}

void UEXOptionsInput::InvertYAxisChanged(bool Val)
{
	Settings->SetInvertYAxis(Val);
}

void UEXOptionsInput::MouseSensChanged(float Val)
{
	MouseSens = SliderValueToSens(Val);
	MouseSensValueText->SetText(FText::FromString(FString::SanitizeFloat(MouseSens)));
	Settings->SetAimSensitivity(MouseSens);
}

void UEXOptionsInput::ScopeSensScaleChanged(float Val)
{
	Settings->SetScopeSensitivityScale(Val);
	ScopeSensScaleValueText->SetText(FText::FromString(FString::SanitizeFloat(Val)));
}

float UEXOptionsInput::SliderValueToSens(float Value)
{
	return Value * (MaxSensitivity - MinSensitivity) + MinSensitivity;
}

float UEXOptionsInput::SensToSliderValue(float Sens)
{
	return (Sens - MinSensitivity) / (MaxSensitivity - MinSensitivity);
}

