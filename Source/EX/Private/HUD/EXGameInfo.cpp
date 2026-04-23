// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXGameInfo.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "HUD/EXOldGames.h"
#include "Online/EXGameInstance.h"

void UEXGameInfo::Init(const FGameInfo& Info, UEXOldGames* InParent)
{
	Parent = InParent;
	GameId = Info.GameId;
	if (Info.PlayerWon == EGameResult::Win)
	{
		RootBorder->SetBrushColor(WinColor);
		GameResult->SetText(WinText);
	}
	else if(Info.PlayerWon == EGameResult::Draw)
	{
		RootBorder->SetBrushColor(DrawColor);
		GameResult->SetText(DrawText);
	}
	else if(Info.PlayerWon == EGameResult::Lose)
	{
		RootBorder->SetBrushColor(LoseColor);
		GameResult->SetText(LoseText);
	}
	if (Info.bRanked)
	{
		Ranked->SetText(RankedText);
	}
	Map->SetText(FText::FromString(Info.Map));
	Date->SetText(FText::FromString(Info.Date));

	RootButton->OnClicked.AddDynamic(this, &UEXGameInfo::OnClicked);
}

void UEXGameInfo::LoadGameResults(const FGameResultSW& InGameResult)
{
	bGameResultLoaded = true;
	MyGameResult = InGameResult;
}

void UEXGameInfo::OnClicked()
{
	if ((GameId == 0))
	{
		return;
	}

	if (bGameResultLoaded)
	{
		Parent->ShowGame(MyGameResult);
	}
	else
	{
		UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
		GI->RequestGameInfo(GameId);
	}
}
