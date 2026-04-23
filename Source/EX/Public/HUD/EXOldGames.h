// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXOldGames.generated.h"

class UVerticalBox;
class UEXGameInfo;
class UEXGameResult;

/**
 * 
 */
UCLASS()
class EX_API UEXOldGames : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(const TArray<FGameInfo>& OldGames);

	void ReceiveGameInfo(uint64 GameId, const FGameResultSW& GameResult);

	void ShowGame(const FGameResultSW& GameResult);

protected:
	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* OldGameList = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEXGameResult* WBP_GameResult = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEXGameInfo> GameInfoClass;

	UPROPERTY()
	TArray<UEXGameInfo*> ListOfGames;
};
