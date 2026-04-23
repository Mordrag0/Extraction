// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXLobbyPlayerRow.generated.h"

class UTextBlock;
class AEXPlayerState;

/**
 * 
 */
UCLASS()
class EX_API UEXLobbyPlayerRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPlayer(AEXPlayerState* PS);
	void ClearPlayer();
	bool IsEmpty() const { return !!Player; }
	bool IsPlayer(AEXPlayerState* PS) const { return (Player == PS); }
protected:
	void SetPlayerName(const FString& PlayerName);
	
	UPROPERTY(BlueprintReadOnly, Category = "Lobby", Meta = (BindWidget))
	UTextBlock* Name = nullptr;

	AEXPlayerState* Player = nullptr;
};
