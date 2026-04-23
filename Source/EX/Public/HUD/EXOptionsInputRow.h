// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXOptionsInputRow.generated.h"

class UInputKeySelector;
class UTextBlock;
class UEXOptionsInput;

/**
 * 
 */
UCLASS()
class EX_API UEXOptionsInputRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(UEXOptionsInput* Owner);
	void Fill(const FName& BindName, const FName& DisplayName, bool bIsInverted, const FKey& Input);

	FKey GetKey() const;
	UInputKeySelector* GetSelector();
protected:
	UFUNCTION()
	void KeyChanged(FInputChord SelectedKey);

	UPROPERTY(Meta = (BindWidget))
	UInputKeySelector* Selector = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* BindText = nullptr;

	FName BindName;
	FName DisplayName;
	bool bInverted;

	UPROPERTY()
	UEXOptionsInput* OwnerWidget = nullptr;

};
