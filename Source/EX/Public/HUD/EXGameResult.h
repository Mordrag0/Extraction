// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXGameResult.generated.h"

class UTextBlock;
class UEXGameResultPlayer;
class UEXGameResultTeamScore;

/**
 * 
 */
UCLASS()
class EX_API UEXGameResult : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(const FGameResultSW& Result);

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Map = nullptr;

	bool bRankedGame;
	EGameResult AttGameResult;

	bool bOwnerOnAttack;

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* AttackerPanel = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* DefenderPanel = nullptr;

	UPROPERTY()
	TArray<UEXGameResultPlayer*> AttackerRows;
	UPROPERTY()
	TArray<UEXGameResultPlayer*> DefenderRows;

	UPROPERTY(Meta = (BindWidget))
	UEXGameResultTeamScore* AttScore = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEXGameResultTeamScore* DefScore = nullptr;
};
