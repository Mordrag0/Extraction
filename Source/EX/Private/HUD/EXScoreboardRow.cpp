// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXScoreboardRow.h"
#include "Online/EXPlayerState.h"
#include "Components/TextBlock.h"
#include "HUD/EXScoreboardBase.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UEXScoreboardRow::Init(UEXScoreboardBase* InParent)
{
	Parent = InParent;
	SetVisibility(ESlateVisibility::Hidden);
}

void UEXScoreboardRow::SetData(AEXPlayerState* Player)
{
	PlayerName->SetText(FText::FromString(Player->GetPlayerName()));
	Kills->SetText(FText::FromString(FString::FromInt(Player->GetKills())));
	Assists->SetText(FText::FromString(FString::FromInt(Player->GetAssists())));
	Deaths->SetText(FText::FromString(FString::FromInt(Player->GetDeaths())));
	Score->SetText(FText::FromString(FString::FromInt(Player->GetPlayerScore())));
	Ping->SetText(FText::FromString(FString::FromInt(Player->GetCompressedPing() << 2)));

	PS = Player;
	SetVisibility(ESlateVisibility::Visible);
}

void UEXScoreboardRow::ClearData()
{
	PlayerName->SetText(FText::GetEmpty());
	Kills->SetText(FText::GetEmpty());
	Assists->SetText(FText::GetEmpty());
	Deaths->SetText(FText::GetEmpty());
	Score->SetText(FText::GetEmpty());
	Ping->SetText(FText::GetEmpty());

	PS = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

FReply UEXScoreboardRow::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (Parent)
	{
		const FGeometry G = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(GetOwningPlayer());
		Parent->RowClicked(this, (InMouseEvent.GetScreenSpacePosition() - G.GetAbsolutePosition()) / G.Scale);
	}

	return Reply;
}
