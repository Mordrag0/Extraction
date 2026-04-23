// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/EXScoreboardBase.h"
#include "EXScoreboardDM.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXScoreboardDM : public UEXScoreboardBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void Show() override;

protected:
	void GetRows(TArray<UEXScoreboardRow*>& Team, UPanelWidget* TeamPanel);

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* Players = nullptr;

	UPROPERTY()
	TArray<UEXScoreboardRow*> PlayerRows;
};
