// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptionsCrosshair.h"
#include "System/EXSettings.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "EX.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"

void UEXOptionsCrosshair::Init(TSharedPtr<FEXSettings> InSettings, bool bReset)
{
	Settings = InSettings;

	if (!bReset)
	{
		PlusGap->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::PGapChanged);
		PlusSize->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::PSizeChanged);
		PlusThickness->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::PThicknessChanged);
		PlusDynamicMultiplier->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::PDynamicMultiplierChanged);
		DotSize->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::DSizeChanged);
		CircleSize->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::CSizeChanged);
		CircleThickness->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::CThicknessChanged);
		CircleDynamicMultiplier->OnTextChanged.AddDynamic(this, &UEXOptionsCrosshair::CDynamicMultiplierChanged);
		CrosshairRed->OnValueChanged.AddDynamic(this, &UEXOptionsCrosshair::RedChanged);
		CrosshairGreen->OnValueChanged.AddDynamic(this, &UEXOptionsCrosshair::GreenChanged);
		CrosshairBlue->OnValueChanged.AddDynamic(this, &UEXOptionsCrosshair::BlueChanged);
		CrosshairAlpha->OnValueChanged.AddDynamic(this, &UEXOptionsCrosshair::AlphaChanged);
		AddCrosshair->OnClicked.AddDynamic(this, &UEXOptionsCrosshair::CrosshairAdded);
		RemoveCrosshair->OnClicked.AddDynamic(this, &UEXOptionsCrosshair::CrosshairRemoved);
		CrosshairList->OnSelectionChanged.AddDynamic(this, &UEXOptionsCrosshair::CrosshairSelected);

		DynamicCrosshair = UWidgetBlueprintLibrary::GetDynamicMaterial(CrosshairImage->Brush);
	}

	Crosshairs = Settings->GetCrosshairs();
	CrosshairList->ClearOptions();
	for (int32 Idx = 0; Idx < Crosshairs.Num(); Idx++)
	{
		CrosshairList->AddOption(FString::FromInt(Idx + 1));
	}
	ActiveCrosshairIdx = Settings->GetActiveCrosshairIdx();
	CrosshairList->SetSelectedOption(FString::FromInt(ActiveCrosshairIdx + 1));
}

void UEXOptionsCrosshair::LoadActiveCrosshair()
{
	const float PGap = Crosshairs[ActiveCrosshairIdx].CP_Gap;
	const float PSize = Crosshairs[ActiveCrosshairIdx].CP_Size;
	const float PThickness = Crosshairs[ActiveCrosshairIdx].CP_Thickness;
	const float PDynamicMultiplier = Crosshairs[ActiveCrosshairIdx].CP_DynamicMultiplier;
	const float DSize = Crosshairs[ActiveCrosshairIdx].CD_Size;
	const float CSize = Crosshairs[ActiveCrosshairIdx].CC_Size;
	const float CThickness = Crosshairs[ActiveCrosshairIdx].CC_Thickness;
	const float CDynamicMultiplier = Crosshairs[ActiveCrosshairIdx].CC_DynamicMultiplier;
	const FLinearColor CX_Color = Crosshairs[ActiveCrosshairIdx].CX_Color;

	PlusGap->SetText(FText::FromString(FString::SanitizeFloat(PGap)));
	PlusSize->SetText(FText::FromString(FString::SanitizeFloat(PSize)));
	PlusThickness->SetText(FText::FromString(FString::SanitizeFloat(PThickness)));
	PlusDynamicMultiplier->SetText(FText::FromString(FString::SanitizeFloat(PDynamicMultiplier)));
	DotSize->SetText(FText::FromString(FString::SanitizeFloat(DSize)));
	CircleSize->SetText(FText::FromString(FString::SanitizeFloat(CSize)));
	CircleThickness->SetText(FText::FromString(FString::SanitizeFloat(CThickness)));
	CircleDynamicMultiplier->SetText(FText::FromString(FString::SanitizeFloat(CDynamicMultiplier)));
	CrosshairRed->SetValue(CX_Color.R / 255.f);
	CrosshairGreen->SetValue(CX_Color.G / 255.f);
	CrosshairBlue->SetValue(CX_Color.B / 255.f);
	CrosshairAlpha->SetValue(CX_Color.A / 255.f);
	RedText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(CX_Color.R * 255))));
	GreenText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(CX_Color.G * 255))));
	BlueText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(CX_Color.B * 255))));
	AlphaText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(CX_Color.A * 255))));

	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_GAP_NAME, PGap);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_SIZE_NAME, PSize);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_THICKNESS_NAME, PThickness);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_DYNAMIC_MULTIPLIER_NAME, PDynamicMultiplier);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_DOT_SIZE_NAME, DSize);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_SIZE_NAME, CSize);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_THICKNESS_NAME, CThickness);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_DYNAMIC_MULTIPLIER_NAME, CDynamicMultiplier);
	DynamicCrosshair->SetVectorParameterValue(CROSSHAIR_PARAM_COLOR_NAME, CX_Color);

	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_EDITOR_WIDTH, CrosshairImageWidth);
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_EDITOR_HEIGHT, CrosshairImageHeight);
}

void UEXOptionsCrosshair::PGapChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CP_Gap = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_GAP_NAME, Value);
}

void UEXOptionsCrosshair::PSizeChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CP_Size = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_SIZE_NAME, Value);
}

void UEXOptionsCrosshair::PThicknessChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CP_Thickness = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_THICKNESS_NAME, Value);
}

void UEXOptionsCrosshair::PDynamicMultiplierChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CP_DynamicMultiplier = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_DYNAMIC_MULTIPLIER_NAME, Value);
}

void UEXOptionsCrosshair::DSizeChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CD_Size = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_DOT_SIZE_NAME, Value);
}

void UEXOptionsCrosshair::CSizeChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CC_Size = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_SIZE_NAME, Value);
}

void UEXOptionsCrosshair::CThicknessChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CC_Thickness = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_THICKNESS_NAME, Value);
}

void UEXOptionsCrosshair::CDynamicMultiplierChanged(const FText& Text)
{
	float Value = FCString::Atof(*Text.ToString());
	Crosshairs[ActiveCrosshairIdx].CC_DynamicMultiplier = Value;
	DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_DYNAMIC_MULTIPLIER_NAME, Value);
}

void UEXOptionsCrosshair::RedChanged(float Value)
{
	Crosshairs[ActiveCrosshairIdx].CX_Color.R = Value;
	DynamicCrosshair->SetVectorParameterValue(CROSSHAIR_PARAM_COLOR_NAME, Crosshairs[ActiveCrosshairIdx].CX_Color);
	RedText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(Value * 255))));
}

void UEXOptionsCrosshair::GreenChanged(float Value)
{
	Crosshairs[ActiveCrosshairIdx].CX_Color.G = Value;
	DynamicCrosshair->SetVectorParameterValue(CROSSHAIR_PARAM_COLOR_NAME, Crosshairs[ActiveCrosshairIdx].CX_Color);
	GreenText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(Value * 255))));
}

void UEXOptionsCrosshair::BlueChanged(float Value)
{
	Crosshairs[ActiveCrosshairIdx].CX_Color.B = Value;
	DynamicCrosshair->SetVectorParameterValue(CROSSHAIR_PARAM_COLOR_NAME, Crosshairs[ActiveCrosshairIdx].CX_Color);
	BlueText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(Value * 255))));
}

void UEXOptionsCrosshair::AlphaChanged(float Value)
{
	Crosshairs[ActiveCrosshairIdx].CX_Color.A = Value;
	DynamicCrosshair->SetVectorParameterValue(CROSSHAIR_PARAM_COLOR_NAME, Crosshairs[ActiveCrosshairIdx].CX_Color);
	AlphaText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(Value * 255))));
}

void UEXOptionsCrosshair::CrosshairAdded()
{
	Crosshairs.Add(FCrosshair());
	CrosshairList->AddOption(FString::FromInt(Crosshairs.Num()));
	CrosshairList->RefreshOptions();
	CrosshairList->SetSelectedOption(FString::FromInt(Crosshairs.Num()));
}

void UEXOptionsCrosshair::CrosshairRemoved()
{
	if (Crosshairs.Num() > 1)
	{
		CrosshairList->RemoveOption(FString::FromInt(Crosshairs.Num()));
		Crosshairs.RemoveAt(ActiveCrosshairIdx + 1);
		CrosshairList->SetSelectedOption(FString::FromInt(1));
		CrosshairList->RefreshOptions();
	}
}

void UEXOptionsCrosshair::CrosshairSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ActiveCrosshairIdx = FCString::Atoi(*SelectedItem) - 1;
	if (ActiveCrosshairIdx >= 0)
	{
		LoadActiveCrosshair();
	}
}
