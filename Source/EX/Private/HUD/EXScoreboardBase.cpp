// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXScoreboardBase.h"

#include "Online/EXTeam.h"
#include "Online/EXPlayerState.h"
#include "EngineUtils.h"
#include "HUD/EXScoreboardRow.h"
#include "Components/PanelWidget.h"
#include "HUD/EXPlayerContextMenu.h"
#include "Online/EXLevelRules.h"
#include "System/EXGameplayStatics.h"
#include "Player/EXBaseController.h"
#include "Online/EXGameModeSW.h"
#include "Online/EXGameModeBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EXTypes.h"
#include "Online/EXGameStateSW.h"
#include "Components/TextBlock.h"

void UEXScoreboardBase::Show()
{
	const AEXGameStateBase* GS = GetWorld()->GetGameState<AEXGameStateBase>();
	if (!GS)
	{
		return;
	}

	// Handle spectators
	TArray<AEXPlayerState*> CurrentSpectators = GS->GetSpectatorMembers();
	TArray<FString> SpectatorNames;
	for (const AEXPlayerState* PS : CurrentSpectators)
	{
		if (PS)
		{
			SpectatorNames.Add(PS->GetPlayerName());
		}
	}
	FString JoinedList = FString::Join(SpectatorNames, TEXT(", "));
	FText SpectatorList = FText::FromString(JoinedList);
	Spectators->SetText(SpectatorList);

	SetVisibility(ESlateVisibility::Visible);
}

void UEXScoreboardBase::RowClicked(UEXScoreboardRow* Row, const FVector2D Position)
{
	if (!Row->GetPlayer())
	{
		return;
	}
	ContextMenu->Show(Row->GetPlayer(), Position);
}

void UEXScoreboardBase::PopulateRows(const TArray<UEXScoreboardRow*>& TeamRows, const TArray<AEXPlayerState*>& TeamMembers) const
{
	for (int32 RowIdx = 0; RowIdx < TeamRows.Num(); RowIdx++)
	{
		if (RowIdx < TeamMembers.Num())
		{
			if (TeamMembers[RowIdx])
			{
				TeamRows[RowIdx]->SetData(TeamMembers[RowIdx]);
			}
		}
		else
		{
			TeamRows[RowIdx]->ClearData();
		}
	}
}

void UEXScoreboardBase::BeginUIMode()
{
	bUIMode = true;
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayer(), this);
}

void UEXScoreboardBase::Close(bool bExitUIMode)
{
	if (bUIMode)
	{
		if (!bExitUIMode)
		{
			return;
		}
		ContextMenu->SetVisibility(ESlateVisibility::Hidden);
		bUIMode = false;
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer());
	}
	SetVisibility(ESlateVisibility::Hidden);
}

FReply UEXScoreboardBase::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey().GetFName().IsEqual(SCOREBOARD_KEY))
	{
		Close(true);
	}

	return Reply;
}

