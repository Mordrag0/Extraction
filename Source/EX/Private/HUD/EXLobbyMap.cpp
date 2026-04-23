// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXLobbyMap.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UEXLobbyMap::Init(const FName& InMap, int32 InIdx)
{
	Map = InMap;
	Idx = InIdx;

	Button->OnClicked.AddDynamic(this, &UEXLobbyMap::OnBtnClicked);
	MapName->SetText(FText::FromString(InMap.ToString()));
}

void UEXLobbyMap::SetVotedByLocal(bool bLocalVoted)
{
	OnVotedByLocal(bLocalVoted);
}

void UEXLobbyMap::SetVotes(int32 Votes)
{
	OnVotesSet(Votes);
}

void UEXLobbyMap::OnBtnClicked()
{
	OnClicked.Execute(Map, Idx);
}
