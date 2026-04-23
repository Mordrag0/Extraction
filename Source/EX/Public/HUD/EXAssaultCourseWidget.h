// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXAssaultCourseWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class EX_API UEXAssaultCourseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init();
protected:
	UFUNCTION()
	void ScoreChanged(int32 NewScore);
	UFUNCTION()
	void TimeUpdated(int32 DeciSeconds);

	UPROPERTY(VisibleAnywhere, Category = "AssaultCourse", Meta = (BindWidget))
	UTextBlock* Score = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "AssaultCourse", Meta = (BindWidget))
	UTextBlock* Timer = nullptr;

	UPROPERTY()
	class AEXAssaultCourseGame* GM = nullptr;
};
