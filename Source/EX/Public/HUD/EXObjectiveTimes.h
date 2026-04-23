// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Online/EXGameStateSW.h"
#include "EXTypes.h"
#include "EXObjectiveTimes.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXObjectiveTimes : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Score")
	void Refresh(const TArray<FRoundScore>& Scores);
};
