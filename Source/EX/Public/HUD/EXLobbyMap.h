// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXLobbyMap.generated.h"

DECLARE_DELEGATE_TwoParams(FMapClicked, const FName&, int32);

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class EX_API UEXLobbyMap : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(const FName& InMap, int32 InIdx);

	FMapClicked OnClicked;

	void SetVotedByLocal(bool bLocalVoted);

	void SetVotes(int32 Votes);

	int32 GetIdx() const { return Idx; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnVotedByLocal(bool bLocalVoted);
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnVotesSet(int32 Votes);

	UFUNCTION()
	void OnBtnClicked();

	UPROPERTY(Meta = (BindWidget))
	UButton* Button = nullptr;

	FName Map;
	int32 Idx = -1;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MapName = nullptr;
};
