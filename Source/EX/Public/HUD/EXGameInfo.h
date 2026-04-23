// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXGameInfo.generated.h"

class UTextBlock;
class UBorder;
class UButton;
class UEXOldGames;

/**
 * 
 */
UCLASS()
class EX_API UEXGameInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(const FGameInfo& Info, UEXOldGames* InParent);
	uint64 GetGameId() const { return GameId; }
	void LoadGameResults(const FGameResultSW& InGameResult);

protected:
	UPROPERTY()
	UEXOldGames* Parent = nullptr;

	bool bGameResultLoaded = false;
	FGameResultSW MyGameResult;

	UFUNCTION()
	void OnClicked();

	UPROPERTY(Meta = (BindWidget))
	UButton* RootButton = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UBorder* RootBorder = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Map = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* GameResult = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Date = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Ranked = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FLinearColor WinColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FLinearColor DrawColor = FLinearColor::Gray;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FLinearColor LoseColor = FLinearColor::Blue;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText WinText = NSLOCTEXT("UEXGameInfo", "Win", "Win");
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText DrawText = NSLOCTEXT("UEXGameInfo", "Draw", "Draw");
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText LoseText = NSLOCTEXT("UEXGameInfo", "Lose", "Lose");
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText RankedText = NSLOCTEXT("UEXGameInfo", "Ranked", "Ranked");

	uint64 GameId = 0;
};
