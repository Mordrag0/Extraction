// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameModeLobby.h"
#include "Player/EXLobbyController.h"
#include "EX.h"
#include "Online/EXGameStateLobby.h"
#include "GameFramework/PlayerState.h"
#include "Online/EXGameModeBase.h"
#include "Online/EXGameInstance.h"
#include "Online/EXPlayerState.h"

AEXGameModeLobby::AEXGameModeLobby()
{
	bUseSeamlessTravel = true;
}

void AEXGameModeLobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	GS->RemovePlayer(Exiting->GetPlayerState<AEXPlayerState>());
}

void AEXGameModeLobby::InitGameState()
{
	Super::InitGameState();

	GS = GetGameState<AEXGameStateLobby>();
	GS->SetUsingTeams(bTeams);

	for (const FName& MapName : ValidMapNames)
	{
		MapOptions.Add(MapName);
	}
	GS->SetMapOptions(MapOptions);
}

void AEXGameModeLobby::Vote(AEXLobbyController* PC, int32 Idx)
{
	if (Voters.Contains(PC)) // PC already voted
	{
		return;
	}
	if (TimeToVoteRemaining <= 0) // Voting is over
	{
		return;
	}
	Voters.Add(PC, Idx);
	GS->Vote(Idx);
}

void AEXGameModeLobby::LoadNewPLayer(uint64 SteamId, const TArray<FSquadMerc>& Squad, const FString& Name)
{
	AEXPlayerState* NewPS = GetPlayer(SteamId);
	NewPS->SetPlayerName(Name);


}

AEXPlayerState* AEXGameModeLobby::GetPlayer(uint64 SteamId) const
{
	for (AEXPlayerState* PS : LobbyPlayers)
	{
		if (PS->GetSteamID() == SteamId)
		{
			return PS;
		}
	}
	UE_LOG(LogEXLevel, Error, TEXT("NO PC FOUND [%llu]"), SteamId);
	return nullptr;
}

void AEXGameModeLobby::GoToMap()
{
	const TArray<int32> Votes = GS->GetVotes();
	int32 Max = 0;
	for (int32 Vote : Votes)
	{
		if (Vote > Max)
		{
			Max = Vote;
		}
	}

	TArray<int32> TopValues;
	for (int32 Idx = 0; Idx < Votes.Num(); Idx++)
	{
		if (Votes[Idx] == Max)
		{
			TopValues.Add(Idx);
		}
	}
	int32 Win = TopValues[FMath::RandRange(0, TopValues.Num() - 1)];
	FString URL = MapOptions[Win].ToString();
	int32 SelectedGameMode = 0;
	URL += "?Game=" + GameModes[SelectedGameMode]->GetPathName();
	FName MapName = MapOptions[Win];
	GetWorld()->ServerTravel(URL, true);
}

FString AEXGameModeLobby::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	FString Ret = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, TEXT(""));

	AEXPlayerState* PS = NewPlayerController->GetPlayerState<AEXPlayerState>();
#if WITH_EDITOR
	static int32 PlayerIdx = 1;
	PS->SetSteamID(PlayerIdx++);
#else
	uint64 SteamId = UniqueId->IsValid() ? (*(uint64*)UniqueId->GetBytes()) : 0;
	PS->SetSteamID(SteamId);
#endif

	int32 Attackers = GS->GetAttackers().Num();
	int32 Defenders = GS->GetDefenders().Num();
	ETeam Team = bJoinToSpec ? ETeam::Spectator : ((Attackers > Defenders) ? ETeam::Defense : ETeam::Attack);
	GS->AddPlayer(PS, Team);

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->RequestPlayerInfo(PS);

	LobbyPlayers.Add(PS);

	return Ret;
}

void AEXGameModeLobby::BeginPlay()
{
	Super::BeginPlay();

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->MSConnect();

	TimeToVoteRemaining = TimeToVote;
	LobbyTimeRemaining = LobbyTime;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Vote, this, &AEXGameModeLobby::Second, 1, true);
}

void AEXGameModeLobby::Second()
{
	TimeToVoteRemaining = FMath::Max(0, TimeToVoteRemaining - 1);
	LobbyTimeRemaining = FMath::Max(0, LobbyTimeRemaining - 1);
	GS->SetTimers(TimeToVoteRemaining, LobbyTimeRemaining);
	if (LobbyTimeRemaining == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Vote);
		GoToMap();
	}
}

