// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXTypes.h"
#include "GameFramework/GameStateBase.h"
#include "EXGameStateLobby.generated.h"

class AEXGameModeLobby;
class AEXLobbyController;
class AEXPlayerState;

/**
 * 
 */
UCLASS()
class EX_API AEXGameStateLobby : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TArray<AEXPlayerState*> GetAllPlayers() const { return AllPlayers; }
	TArray<AEXPlayerState*> GetAttackers() const { return Attackers; }
	TArray<AEXPlayerState*> GetDefenders() const { return Defenders; }

	void Vote(int32 Idx);

	void AddPlayer(AEXPlayerState* PS, ETeam Team);
	void RemovePlayer(AEXPlayerState* PS);

	void SetUsingTeams(bool bInTeams) { bTeams = bInTeams; }

	void SetMapOptions(const TArray<FName>& InMapOptions) { MapOptions = InMapOptions; }

	TArray<int32> GetVotes() const { return Votes; }
	void SetTimers(int32 TimeToVoteRemaining, int32 LobbyTimeRemaining);
protected:
	virtual void BeginPlay() override;

	AEXGameModeLobby* GM = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_Players)
	TArray<AEXPlayerState*> AllPlayers;
	UPROPERTY(ReplicatedUsing = OnRep_Players)
	TArray<AEXPlayerState*> Attackers;
	UPROPERTY(ReplicatedUsing = OnRep_Players)
	TArray<AEXPlayerState*> Defenders;
	UPROPERTY(ReplicatedUsing = OnRep_Players)
	TArray<AEXPlayerState*> Spectators;

	UFUNCTION()
	void OnRep_Players();

	UPROPERTY(ReplicatedUsing = OnRep_Players)
	bool bTeams = false;

	UPROPERTY()
	AEXLobbyController* LocalPC = nullptr;

	UPROPERTY(ReplicatedUsing=OnRep_Votes)
	TArray<int32> Votes;
	UFUNCTION()
	void OnRep_Votes();

	UPROPERTY(ReplicatedUsing = OnRep_MapOptions)
	TArray<FName> MapOptions;
	UFUNCTION()
	void OnRep_MapOptions();

	UPROPERTY(ReplicatedUsing = OnRep_VoteTime)
	int32 VoteTime = 0;
	UFUNCTION()
	void OnRep_VoteTime();
	UPROPERTY(ReplicatedUsing = OnRep_LobbyTime)
	int32 LobbyTime = 0;
	UFUNCTION()
	void OnRep_LobbyTime();

};
