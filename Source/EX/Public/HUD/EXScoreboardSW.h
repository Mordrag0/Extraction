// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXScoreboardBase.h"
#include "EXScoreboardSW.generated.h"

class UEXObjectiveTimes;


/**
 * 
 */
UCLASS()
class EX_API UEXScoreboardSW : public UEXScoreboardBase
{
	GENERATED_BODY()

public:
	virtual void Show() override;

	UEXObjectiveTimes* GetObjectiveBox() const { return Objectives; }

protected:
	virtual void NativeConstruct() override;

	void GetRows(TArray<UEXScoreboardRow*>& Team, UPanelWidget* TeamPanel);

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* TopTeam = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* BottomTeam = nullptr;

	UPROPERTY()
	TArray<UEXScoreboardRow*> TopRows;
	UPROPERTY()
	TArray<UEXScoreboardRow*> BottomRows;

	UPROPERTY(Meta = (BindWidgetOptional))
	UEXObjectiveTimes* Objectives = nullptr;

};
