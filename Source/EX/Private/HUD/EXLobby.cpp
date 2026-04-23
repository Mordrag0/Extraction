// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXLobby.h"
#include "Components/PanelWidget.h"
#include "HUD/EXLobbyPlayerRow.h"
#include "Player/EXLobbyController.h"
#include "EXTypes.h"
#include "System/EXGameplayStatics.h"
#include "HUD/EXLobbyMap.h"
#include "Components/VerticalBox.h"
#include "Components/UniformGridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Online/EXPlayerState.h"


void UEXLobby::RefreshPlayers(bool bTeams, TArray<AEXPlayerState*> AllPlayers, TArray<AEXPlayerState*> Attackers, TArray<AEXPlayerState*> Defenders, TArray<AEXPlayerState*> Spectators)
{
	TeamsRoot->SetVisibility(bTeams ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	AllPlayersPanel->SetVisibility(!bTeams ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	InitToEmpty();
	if (bTeams)
	{
		Fill(AttackerRows, Attackers);
		Fill(DefenderRows, Defenders);
	}
	else
	{
		Fill(AllPlayersRows, AllPlayers);
	}
	Fill(SpectatorRows, Spectators);
}

void UEXLobby::SetMapOptions(const TArray<FName>& MapOptions)
{
	check(MapClass);
	int32 Idx = 0;
	for (const FName& MapName : MapOptions)
	{
		UEXLobbyMap* MapOptionWidget = CreateWidget<UEXLobbyMap>(this, MapClass);
		MapOptionWidget->Init(MapName, Idx++);
		UUniformGridSlot* MapSlot = MapOptionBox->AddChildToUniformGrid(MapOptionWidget, 
			MapOptionBox->GetChildrenCount() - 1 /* There is a sizebox in it that we don't want to count.*/, 0);
		MapSlot->SetHorizontalAlignment(HAlign_Fill);
		MapSlot->SetVerticalAlignment(VAlign_Fill);
		MapWidgets.Add(MapName, MapOptionWidget);
		MapOptionWidget->OnClicked.BindUObject(this, &UEXLobby::OnMapClicked);
	}
}

void UEXLobby::NativeConstruct()
{
	Super::NativeConstruct();

	InitToEmpty();
	PC = GetOwningPlayer<AEXLobbyController>();
}

void UEXLobby::Fill(TArray<UEXLobbyPlayerRow*> PlayersRows, TArray<AEXPlayerState*> Players)
{
	int32 Num = FMath::Min(PlayersRows.Num(), Players.Num());
	for (int32 Idx = 0; Idx < Num; Idx++)
	{
		if (Players[Idx])
		{
			PlayersRows[Idx]->SetPlayer(Players[Idx]);
		}
	}
}

void UEXLobby::InitToEmpty()
{
	AllPlayersRows = UEXGameplayStatics::GetPanelChildren<UEXLobbyPlayerRow>(AllPlayersPanel);
	for (UEXLobbyPlayerRow* Row : AllPlayersRows)
	{
		Row->ClearPlayer();
	}

	AttackerRows = UEXGameplayStatics::GetPanelChildren<UEXLobbyPlayerRow>(AttackerPanel);
	for (UEXLobbyPlayerRow* Row : AttackerRows)
	{
		Row->ClearPlayer();
	}

	DefenderRows = UEXGameplayStatics::GetPanelChildren<UEXLobbyPlayerRow>(DefenderPanel);
	for (UEXLobbyPlayerRow* Row : DefenderRows)
	{
		Row->ClearPlayer();
	}

	SpectatorRows = UEXGameplayStatics::GetPanelChildren<UEXLobbyPlayerRow>(SpectatorPanel);
	for (UEXLobbyPlayerRow* Row : SpectatorRows)
	{
		Row->ClearPlayer();
	}
}

void UEXLobby::OnMapClicked(const FName& Map, int32 Idx)
{
	if (bVoted) // Already voted
	{
		return;
	}
	if (VoteTimeRemaining <= 0) // Vote over
	{
		return;
	}
	bVoted = true;
	Vote = Map;
	MapWidgets[Map]->SetVotedByLocal(true);
	
	PC->Server_Reliable_Vote(Idx);
}

void UEXLobby::SetVotes(const TArray<int32>& Votes)
{
	for (const auto& KVP : MapWidgets)
	{
		KVP.Value->SetVotes(Votes[KVP.Value->GetIdx()]);
	}
}

void UEXLobby::SetVoteTimeRemaining(int32 VoteTime)
{
	VoteTimeRemaining = VoteTime;
	VoteSecondsRemaining->SetText(FText::FromString(FString::FromInt(VoteTime)));
}

void UEXLobby::SetLobbyTimeRemaining(int32 LobbyTime)
{
	LobbySecondsRemaining->SetText(FText::FromString(FString::FromInt(LobbyTime)));
}
