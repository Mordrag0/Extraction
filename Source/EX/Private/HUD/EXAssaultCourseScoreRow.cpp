// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXAssaultCourseScoreRow.h"
#include "Components/TextBlock.h"
#include "UObject/NameTypes.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "System/EXGameplayStatics.h"
#include "Components/Image.h"

void UEXAssaultCourseScoreRow::FillScore(const FAssaultCoursePlayerStat& Stats)
{
	SteamId = UEXGameplayStatics::CreateSteamId(Stats.SteamId);
	if (!Stats.Name.IsNone())
	{
		Name->SetText(FText::FromString(Stats.Name.ToString()));
	}
	else
	{
		if (SteamId.IsValid())
		{
			UAdvancedSteamFriendsLibrary::RequestSteamFriendInfo(SteamId, true);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SteamName, this, &UEXAssaultCourseScoreRow::FillName, 1.f, true);
		}
	}
	Score->SetText(FText::FromString(FString::FromInt(Stats.Score)));
}

void UEXAssaultCourseScoreRow::FillScore(int32 InScore, UTexture2D* Icon)
{
	Score->SetText(FText::FromString(FString::FromInt(InScore)));
	if (Icon)
	{
		check(MedalIcon);
		MedalIcon->SetBrushFromTexture(Icon);
		MedalIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		MedalIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEXAssaultCourseScoreRow::FillName()
{
	FString SteamName = UAdvancedSteamFriendsLibrary::GetSteamPersonaName(SteamId);
	if(!SteamName.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SteamName);
		Name->SetText(FText::FromString(SteamName));
	}
}
