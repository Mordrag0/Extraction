// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXProfileStats.generated.h"

class UUniformGridPanel;

/**
 * 
 */
UCLASS()
class EX_API UEXProfileStats : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetData(const FPlayerStats& PlayerStats);

	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* MercsPanel = nullptr;

	/*UPROPERTY()
	TArray<UEXMercStats*> Mercs;*/

protected:
	virtual void NativeConstruct() override;

public:
	void LoadStats(const FPlayerStats& PlayerStats);
};
