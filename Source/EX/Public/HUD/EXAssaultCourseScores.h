// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXAssaultCourseScores.generated.h"

class UEXAssaultCourseScoreRow;
class UTexture2D;
class UImage;

/**
 * 
 */
UCLASS()
class EX_API UEXAssaultCourseScores : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void Show();
	void Hide();

	void LoadScore(const FAssaultCourseStats& Scores);
	void ShowResult(float TotalScore, bool bPersonalBest, UTexture2D* Icon);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Score")
	int32 NumRows = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Score")
	TSubclassOf<UEXAssaultCourseScoreRow> RowClass;

	UPROPERTY(Meta = (BindWidget))
	class UVerticalBox* Box = nullptr;

	UPROPERTY(Meta = (BindWidget))
	UEXAssaultCourseScoreRow* LastScore = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEXAssaultCourseScoreRow* PersonalBest = nullptr;

	virtual void NativeConstruct() override;


	TArray<UEXAssaultCourseScoreRow*> Rows;
};
