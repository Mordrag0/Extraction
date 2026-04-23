// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXMenuError.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class EX_API UEXMenuError : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Error(FText Message, bool bFatalError);

protected:
	UFUNCTION()
	void Close();

	UPROPERTY(Meta = (BindWidget))
	UButton* CloseBtn = nullptr;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MessageBlock = nullptr;

	virtual void NativeConstruct() override;

	bool bFatal = false;
	bool bOpen = false;
};
