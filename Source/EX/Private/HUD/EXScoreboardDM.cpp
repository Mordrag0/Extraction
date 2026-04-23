// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXScoreboardDM.h"
#include "HUD/EXScoreboardRow.h"
#include "System/EXGameplayStatics.h"
#include "Online/EXGameStateDM.h"
#include "Online/EXPlayerState.h"

void UEXScoreboardDM::Show()
{
	Super::Show();

	const AEXGameStateDM* GS = GetWorld()->GetGameState<AEXGameStateDM>();
	TArray<AEXPlayerState*> EXPSs;
	for (APlayerState* PS : GS->PlayerArray)
	{
		EXPSs.Add(Cast<AEXPlayerState>(PS));
	}
	PopulateRows(PlayerRows, EXPSs);
}

void UEXScoreboardDM::NativeConstruct()
{
	Super::NativeConstruct();

	GetRows(PlayerRows, Players);
}

void UEXScoreboardDM::GetRows(TArray<UEXScoreboardRow*>& Team, UPanelWidget* TeamPanel)
{
	TArray<UEXScoreboardRow*> Rows = UEXGameplayStatics::GetPanelChildren<UEXScoreboardRow>(TeamPanel, true);

	for (UEXScoreboardRow* Row : Rows)
	{
		Team.Add(Row);
		Row->Init(this);
	}
}
