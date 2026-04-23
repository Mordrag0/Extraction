// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXLobbyPlayerRow.h"
#include "Player/EXLobbyController.h"
#include "Components/TextBlock.h"
#include "Online/EXPlayerState.h"

void UEXLobbyPlayerRow::SetPlayer(AEXPlayerState* PC)
{
	Player = PC;
	Player->OnNameChanged.BindUObject(this, &UEXLobbyPlayerRow::SetPlayerName);
	SetPlayerName(Player->GetPlayerName());
}

void UEXLobbyPlayerRow::ClearPlayer()
{
	if (Player)
	{
		Player->OnNameChanged.Unbind();
	}
	Player = nullptr;
	Name->SetText(FText());
}

void UEXLobbyPlayerRow::SetPlayerName(const FString& PlayerName)
{
	Name->SetText(FText::FromString(PlayerName));
}

