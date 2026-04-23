// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXGameResultPlayer.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "System/EXGameplayStatics.h"

void UEXGameResultPlayer::Init(const FPlayerStats& PlayerStat)
{
	bInitialized = true;
	// #EXTODO2
	TArray<UTextBlock*> Fields = UEXGameplayStatics::GetPanelChildren<UTextBlock>(Root);
	Fields[0]->SetText(FText::FromString(PlayerStat.Name));
	Fields[1]->SetText(FText::FromString(FString::FromInt(PlayerStat.Kills)));
	Fields[3]->SetText(FText::FromString(FString::FromInt(PlayerStat.Deaths)));
	Fields[4]->SetText(FText::FromString(FString::FromInt(PlayerStat.TotalExp)));
}

void UEXGameResultPlayer::Clear()
{
	bInitialized = false;
	TArray<UTextBlock*> Fields = UEXGameplayStatics::GetPanelChildren<UTextBlock>(Root);
	for (UTextBlock* Field : Fields)
	{
		Field->SetText(FText());
	}
}
