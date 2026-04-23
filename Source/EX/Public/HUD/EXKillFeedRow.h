// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXNotificationRow.h"
#include "EXKillFeedRow.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class EX_API UEXKillFeedRow : public UEXNotificationRow
{
	GENERATED_BODY()
	
public:
	void InitRow(const FString& DeadName, UTexture2D* KillIcon, const FString& KillName, const FString& PlayerName);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UTextBlock* Dead = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UImage* Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UTextBlock* Kill = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	class UBorder* HighlightBorder = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	class UTexture2D* DefaultIcon = nullptr;
};
