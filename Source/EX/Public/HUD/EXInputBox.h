// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXInputBox.generated.h"

class UEditableTextBox;

/**
 * 
 */
UCLASS()
class EX_API UEXInputBox : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void Show(bool bInTeam);
	void Hide();

	FORCEINLINE UEditableTextBox* GetTextBox() const { return TextBox; }

	FORCEINLINE bool GetTeam() const { return bTeam; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Message", Meta = (BindWidget))
	UEditableTextBox* TextBox = nullptr;

	bool bTeam = false;
};
