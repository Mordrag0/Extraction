// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class UEXTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Timer")
	FString SecondsToString() const;
	
	UFUNCTION()
	void UpdateTimer(int32 InSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category = "Timer")
	void OnTimerUpdated();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timer")
	int32 Seconds;
};
