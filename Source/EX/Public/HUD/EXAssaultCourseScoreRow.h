// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "BlueprintDataDefinitions.h"
#include "EXAssaultCourseScoreRow.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class EX_API UEXAssaultCourseScoreRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void FillScore(const FAssaultCoursePlayerStat& Stats);
	void FillScore(int32 InScore, UTexture2D* Icon);

	UPROPERTY(BlueprintReadOnly, Category = "ACRow", Meta = (BindWidget))
	UTextBlock* Name = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "ACRow", Meta = (BindWidget))
	UTextBlock* Score = nullptr;

protected:
	UFUNCTION()
	void FillName();

	UPROPERTY(Meta = (BindWidget))
	UImage* MedalIcon = nullptr;

	FTimerHandle TimerHandle_SteamName;
	FBPUniqueNetId SteamId;
};
