// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXLobbyController.h"
#include "Net/UnrealNetwork.h"
#include "HUD/EXLobby.h"
#include "Online/EXGameModeLobby.h"


AEXLobbyController::AEXLobbyController()
{
	bShowMouseCursor = true;
}

void AEXLobbyController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GM = GetWorld()->GetAuthGameMode<AEXGameModeLobby>();
	}
	else
	{
		check(LobbyClass);
		Lobby = CreateWidget<UEXLobby>(this, LobbyClass);
		if (Lobby)
		{
			Lobby->AddToViewport();
		}
	}
}

void AEXLobbyController::Server_Reliable_Vote_Implementation(int32 Idx)
{
	GM->Vote(this, Idx);
}
