// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameStateLobby.h"
#include "Online/EXGameModeLobby.h"
#include "EXTypes.h"
#include "Net/UnrealNetwork.h"
#include "Player/EXLobbyController.h"
#include "HUD/EXLobby.h"
#include "Online/EXPlayerState.h"

void AEXGameStateLobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXGameStateLobby, AllPlayers);
	DOREPLIFETIME(AEXGameStateLobby, Attackers);
	DOREPLIFETIME(AEXGameStateLobby, Defenders);
	DOREPLIFETIME(AEXGameStateLobby, bTeams);
	DOREPLIFETIME(AEXGameStateLobby, Votes);
	DOREPLIFETIME_CONDITION(AEXGameStateLobby, MapOptions, COND_InitialOnly);
	DOREPLIFETIME(AEXGameStateLobby, VoteTime);
	DOREPLIFETIME(AEXGameStateLobby, LobbyTime);
}

void AEXGameStateLobby::Vote(int32 Idx)
{
	Votes[Idx]++;
}

void AEXGameStateLobby::AddPlayer(AEXPlayerState* PS, ETeam Team)
{
	AllPlayers.Add(PS);
	if (Team == ETeam::Attack)
	{
		Attackers.Add(PS);
	}
	else if (Team == ETeam::Defense)
	{
		Defenders.Add(PS);
	}
}

void AEXGameStateLobby::RemovePlayer(AEXPlayerState* PS)
{
	AllPlayers.Add(PS);
	Attackers.Add(PS);
	Defenders.Add(PS);
}

void AEXGameStateLobby::SetTimers(int32 TimeToVoteRemaining, int32 LobbyTimeRemaining)
{
	VoteTime = TimeToVoteRemaining;
	LobbyTime = LobbyTimeRemaining;
}

void AEXGameStateLobby::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GM = GetWorld()->GetAuthGameMode<AEXGameModeLobby>();
		int32 NumVoteOptions = GM->GetNumVoteOptions();
		Votes.SetNum(NumVoteOptions, false);
	}
	else
	{
		LocalPC = GetWorld()->GetFirstPlayerController<AEXLobbyController>();
	}
}

void AEXGameStateLobby::OnRep_Players()
{
	if (!LocalPC || !LocalPC->GetLobbyWidget())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AEXGameStateLobby::OnRep_Players);
		return;
	}
	LocalPC->GetLobbyWidget()->RefreshPlayers(bTeams, AllPlayers, Attackers, Defenders, Spectators);
}

void AEXGameStateLobby::OnRep_Votes()
{
	LocalPC->GetLobbyWidget()->SetVotes(Votes);
}

void AEXGameStateLobby::OnRep_MapOptions()
{
	LocalPC->GetLobbyWidget()->SetMapOptions(MapOptions);
}

void AEXGameStateLobby::OnRep_VoteTime()
{
	if (!LocalPC || !LocalPC->GetLobbyWidget())
	{
		return;
	}
	LocalPC->GetLobbyWidget()->SetVoteTimeRemaining(VoteTime);
}

void AEXGameStateLobby::OnRep_LobbyTime()
{
	if (!LocalPC || !LocalPC->GetLobbyWidget())
	{
		return;
	}
	LocalPC->GetLobbyWidget()->SetLobbyTimeRemaining(LobbyTime);
}

