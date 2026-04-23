// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetHealth(float InPercent);

	void Show();
	void Hide();
protected:

	UPROPERTY(EditDefaultsOnly, Category = "HealthBar", Meta = (BindWidget))
	class UProgressBar* HealthProgressbar = nullptr;
};
