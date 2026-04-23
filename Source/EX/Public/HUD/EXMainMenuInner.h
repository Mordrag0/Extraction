// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXMainMenuInner.generated.h"

class UTextBlock;
class UEditableText;
class UEXMainMenu;

/**
 * 
 */
UCLASS()
class EX_API UEXMainMenuInner : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(UEXMainMenu* InOwner, uint64 Exp, uint64 Credits);
	void SetName(const FText& InName);
protected:

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ExpText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CreditsText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableText* NameText = nullptr;

	UPROPERTY(BlueprintReadonly, Category = "Menu")
	UEXMainMenu* Owner = nullptr;
};
