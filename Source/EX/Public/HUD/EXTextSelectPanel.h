// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "Player/EXPlayerController.h"
#include "EXTextSelectPanel.generated.h"

class UTextBlock;
class UUniformGridPanel;

/**
 * 
 */
UCLASS()
class EX_API UEXTextSelectPanel : public UEXHUDElement
{
	GENERATED_BODY()

public:

	void Open() { bOpen = true; }
	bool IsOpen() const { return bOpen; }
	void Close();

	void SetSelectedCategory(const FTextPanelCategory& InCurrentCategory);
	FTextPanelCategory GetCurrentCategory() const { return CurrentCategory; }
protected:

	bool bOpen = false;

	UPROPERTY()
	TArray<UTextBlock*> Lines;

	FTextPanelCategory CurrentCategory;

	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	class UVerticalBox* TextPanel = nullptr;
};
