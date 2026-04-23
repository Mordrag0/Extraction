// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXKillFeedRow.h"
#include "Engine/Texture.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/Border.h"

void UEXKillFeedRow::InitRow(const FString& DeadName, UTexture2D* KillIcon, const FString& KillName, const FString& PlayerName)
{
	Dead->SetText(FText::FromString(DeadName));
	Kill->SetText(FText::FromString(KillName));

	if (KillIcon)
	{
		Icon->SetBrushFromTexture(KillIcon);
	}
	else
	{
		Icon->SetBrushFromTexture(DefaultIcon);
	}

	if (HighlightBorder)
	{
		if (PlayerName == DeadName)
		{
			HighlightBorder->SetBrushColor(FLinearColor(1.f, 0.f, 0.f, 0.4f));
		}
		else if (PlayerName == KillName)
		{
			HighlightBorder->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.4f));
		}
		else
		{
			HighlightBorder->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
		}
	}
}
