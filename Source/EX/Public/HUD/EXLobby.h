// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXLobby.generated.h"

class AEXLobbyController;
class UEXLobbyPlayerRow;
class UPanelWidget;
class UEXLobbyMap;
class UUniformGridPanel;
class UTextBlock;
class AEXPlayerState;

/**
 * 
 */
UCLASS()
class EX_API UEXLobby : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void RefreshPlayers(
		bool bTeams,
		TArray<AEXPlayerState*> AllPlayers,
		TArray<AEXPlayerState*> Attackers,
		TArray<AEXPlayerState*> Defenders,
		TArray<AEXPlayerState*> Spectators
		);

	void SetMapOptions(const TArray<FName>& MapOptions);
	void SetVotes(const TArray<int32>& Votes);
	void SetVoteTimeRemaining(int32 VoteTime);
	void SetLobbyTimeRemaining(int32 LobbyTime);
protected:
	void Fill(TArray<UEXLobbyPlayerRow*> PlayersRows, TArray<AEXPlayerState*> Players);
	void InitToEmpty();

	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* AllPlayersPanel = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* AttackerPanel = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* DefenderPanel = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* SpectatorPanel = nullptr;

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* TeamsRoot = nullptr;

	TArray<UEXLobbyPlayerRow*> AllPlayersRows;
	TArray<UEXLobbyPlayerRow*> AttackerRows;
	TArray<UEXLobbyPlayerRow*> DefenderRows;
	TArray<UEXLobbyPlayerRow*> SpectatorRows;

	UFUNCTION()
	void OnMapClicked(const FName& Map, int32 Idx);

	TMap<FName, UEXLobbyMap*> MapWidgets;

	bool bVoted = false;
	FName Vote;
	int32 VoteTimeRemaining = 0;

	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* MapOptionBox = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<UEXLobbyMap> MapClass;

	AEXLobbyController* PC = nullptr;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* VoteSecondsRemaining = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* LobbySecondsRemaining = nullptr;
};
