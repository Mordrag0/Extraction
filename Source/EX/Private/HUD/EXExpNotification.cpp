// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXExpNotification.h"
#include "HUD/EXHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UEXExpNotification::InitRow(int32 Score, EScoreType Type)
{
	FText Text = FText::FromString(FString::FromInt(Score));
	ExpText->SetText(Text);
	ColoredBorder->SetBrushColor(Colors[Type]);
}
