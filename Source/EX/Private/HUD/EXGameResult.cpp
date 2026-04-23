// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXGameResult.h"
#include "Components/TextBlock.h"
#include "System/EXGameplayStatics.h"
#include "HUD/EXGameResultPlayer.h"
#include "HUD/EXGameResultTeamScore.h"

void UEXGameResult::Init(const FGameResultSW& Result)
{
	Map->SetText(FText::FromString(Result.Map));
	bRankedGame = Result.bRanked;
	AttGameResult = Result.AttGameResult;

	uint64 OwnerSteamId = UEXGameplayStatics::GetLocalSteamID(this);
	bOwnerOnAttack = true;
	for (const FPlayerStats& Player : Result.Def.Players)
	{
		if (Player.SteamId == OwnerSteamId)
		{
			bOwnerOnAttack = false;
			break;
		}
	}

	AttackerRows = UEXGameplayStatics::GetPanelChildren<UEXGameResultPlayer>(AttackerPanel, false);
	const int32 NumAttackers = Result.Att.Players.Num();
	for (int32 Player = 0; Player < NumAttackers; Player++)
	{
		AttackerRows[Player]->Init(Result.Att.Players[Player]);
	}
	for (int32 Player = NumAttackers; Player < AttackerRows.Num(); Player++)
	{
		AttackerRows[Player]->Clear();
	}

	DefenderRows = UEXGameplayStatics::GetPanelChildren<UEXGameResultPlayer>(DefenderPanel, false);
	const int32 NumDefenders = Result.Def.Players.Num();
	for (int32 Player = 0; Player < NumDefenders; Player++)
	{
		DefenderRows[Player]->Init(Result.Def.Players[Player]);
	}
	for (int32 Player = NumDefenders; Player < DefenderRows.Num(); Player++)
	{
		DefenderRows[Player]->Clear();
	}

	AttScore->Init(Result.Att.ObjectiveTimes);
	if (AttGameResult != EGameResult::InProgress)
	{
		DefScore->Init(Result.Def.ObjectiveTimes);
	}
}
