// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXTypes.h"
#include "Blueprint/UserWidget.h"
#include "EXHUDVote.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXHUDVote : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UEXHUDVote(const FObjectInitializer& ObjectInitializer);
	void StartVote(const FText& InText, int32 InDuration);

	void UpdateVotes(int32 InYeses, int32 InNos);

	void Vote(bool bVote);
	void EndVote();
protected:

	UFUNCTION()
	void Refresh();

	bool bVoted = false;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	class UTextBlock* Message = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	class UTextBlock* BindInfo = nullptr;

	FText Text;
	int32 Duration;
	int32 TotalDuration;
	int32 Yeses = 1;
	int32 Nos = 1;

	FTimerHandle TimerHandle_RefreshVote;

	FText BindInfoText;
};
