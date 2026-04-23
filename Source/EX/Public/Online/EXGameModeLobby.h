// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EXTypes.h"
#include "EXGameModeLobby.generated.h"

class AEXGameStateLobby;
class APlayerState;
class AEXLobbyController;
class AEXGameModeBase;
class AEXPlayerState;

/**
 * 
 */
UCLASS()
class EX_API AEXGameModeLobby : public AGameModeBase
{
	GENERATED_BODY()
public:
	AEXGameModeLobby();

	virtual void Logout(AController* Exiting) override;

	virtual void InitGameState() override;

	void Vote(AEXLobbyController* PC, int32 Idx);

	int32 GetNumVoteOptions() const { return MapOptions.Num(); }

	void LoadNewPLayer(uint64 SteamId, const TArray<FSquadMerc>& Squad, const FString& Name);
protected:
	AEXPlayerState* GetPlayer(uint64 SteamId) const;

	void GoToMap();

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	TArray<AEXPlayerState*> LobbyPlayers;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	bool bJoinToSpec = false;

	TArray<FName> MapOptions;

	virtual void BeginPlay() override;

	AEXGameStateLobby* GS = nullptr;

	TMap<AEXLobbyController*, int32> Voters;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	bool bTeams = false;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 TimeToVote = 10;
	int32 TimeToVoteRemaining = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 LobbyTime = 15;
	int32 LobbyTimeRemaining = 0;

	FTimerHandle TimerHandle_Vote;

	UFUNCTION()
	void Second();

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TArray<TSubclassOf<AEXGameModeBase>> GameModes;
};
