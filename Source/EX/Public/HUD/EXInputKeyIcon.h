// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXInputKeyIcon.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class EX_API UEXInputKeyIcon : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	void NativeConstruct() override;

	UFUNCTION()
	void Update();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (BindWidget))
	UTextBlock* TextIcon = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	FName Bind;
};
