// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXAbilityWidget.generated.h"

class UEXInventory;
class UImage;
class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class EX_API UEXAbilityWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void InitializeHUD(UEXInventory* Ability);
	void SetPercent(float Value);
protected:

	UPROPERTY(Meta = (BindWidget))
	UImage* Icon = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* DisplayName = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* RecoverProgress = nullptr;
	

};
