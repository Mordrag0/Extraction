// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOldGames.h"
#include "Components/VerticalBox.h"
#include "HUD/EXGameInfo.h"
#include "Components/VerticalBoxSlot.h"
#include "HUD/EXGameResult.h"

void UEXOldGames::Init(const TArray<FGameInfo>& OldGames)
{
	for (const FGameInfo& OldGame : OldGames)
	{
		UEXGameInfo* GameInfo = CreateWidget<UEXGameInfo>(this, GameInfoClass);
		GameInfo->Init(OldGame, this);
		OldGameList->AddChild(GameInfo);
		ListOfGames.Add(GameInfo);
	}
}

void UEXOldGames::ReceiveGameInfo(uint64 GameId, const FGameResultSW& GameResult)
{
	for (UEXGameInfo* GameInfo : ListOfGames)
	{
		if (GameInfo->GetGameId() == GameId)
		{
			GameInfo->LoadGameResults(GameResult);
			break;
		}
	}
	ShowGame(GameResult);
}

void UEXOldGames::ShowGame(const FGameResultSW& GameResult)
{
	WBP_GameResult->Init(GameResult);
}
