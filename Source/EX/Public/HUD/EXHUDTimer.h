// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "EXHUDTimer.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXHUDTimer : public UEXHUDElement
{
	GENERATED_BODY()
	
public:
	void SetText(FText Text);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void DeleteText();
protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	class UTextBlock* TimerText;
};
