// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXScoreboardRow.generated.h"

class AEXPlayerState;
class UTextBlock;
class UEXScoreboardBase;

/**
 * 
 */
UCLASS()
class EX_API UEXScoreboardRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(UEXScoreboardBase* InParent);
	void SetData(AEXPlayerState* Player);
	void ClearData();

	AEXPlayerState* GetPlayer() const { return PS; }
protected:
	UEXScoreboardBase* Parent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard", Meta = (BindWidget))
	UTextBlock* PlayerName = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard", Meta = (BindWidget))
	UTextBlock* Kills = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard", Meta = (BindWidget))
	UTextBlock* Assists = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard", Meta = (BindWidget))
	UTextBlock* Deaths = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard", Meta = (BindWidget))
	UTextBlock* Score = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard", Meta = (BindWidget))
	UTextBlock* Ping = nullptr;

	AEXPlayerState* PS = nullptr;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

};
