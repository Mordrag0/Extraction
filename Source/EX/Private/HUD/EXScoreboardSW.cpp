// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/EXScoreboardSW.h"
#include "Components/TextBlock.h"
#include "Online/EXPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Online/EXLevelRules.h"
#include "Online/EXTeam.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EXTypes.h"
#include "Online/EXGameStateSW.h"
#include "HUD/EXObjectiveTimes.h"
#include "EX.h"
#include "HUD/EXPlayerContextMenu.h"
#include "System/EXGameplayStatics.h"
#include "Player/EXBaseController.h"
#include "HUD/EXScoreboardRow.h"



void UEXScoreboardSW::Show()
{
	Super::Show();

	const AEXGameStateSW* GS = GetWorld()->GetGameState<AEXGameStateSW>();
	const TArray<FRoundScore> RoundScores = GS->GetRoundScores();
	GetObjectiveBox()->Refresh(RoundScores);

	const AEXTeam* Attackers = GS->GetAttackers();
	const AEXTeam* Defenders = GS->GetDefenders();
	if (!Attackers || !Defenders)
	{
		return;
	}

	AEXBaseController* LocalPC = GetOwningPlayer<AEXBaseController>();
	AEXPlayerState* LocalPS = LocalPC ? LocalPC->GetPS() : nullptr;
	bool bOwnerOnDefense = !!LocalPS && Defenders->GetMembers().Contains(LocalPS);

	if (Attackers && Defenders)
	{
		PopulateRows(TopRows, bOwnerOnDefense ? Defenders->GetMembers() : Attackers->GetMembers());
		PopulateRows(BottomRows, bOwnerOnDefense ? Attackers->GetMembers() : Defenders->GetMembers());
	}
}

void UEXScoreboardSW::NativeConstruct()
{
	Super::NativeConstruct();

	GetRows(TopRows, TopTeam);
	GetRows(BottomRows, BottomTeam);
}

void UEXScoreboardSW::GetRows(TArray<UEXScoreboardRow*>& Team, UPanelWidget* TeamPanel)
{
	TArray<UEXScoreboardRow*> Rows = UEXGameplayStatics::GetPanelChildren<UEXScoreboardRow>(TeamPanel, true);

	for (UEXScoreboardRow* Row : Rows)
	{
		Team.Add(Row);
		Row->Init(this);
	}
}
