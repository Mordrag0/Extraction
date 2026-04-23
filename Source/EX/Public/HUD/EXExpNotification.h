// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXNotificationRow.h"
#include "EXExpNotification.generated.h"

class UBorder;
class UTextBlock;
class UEXHUDWidget;

enum class EScoreType : uint8;

/**
 * 
 */
UCLASS()
class EX_API UEXExpNotification : public UEXNotificationRow
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void InitRow(int32 Score, EScoreType Type);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UTextBlock* ExpText = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "HUD", Meta = (BindWidget))
	UBorder* ColoredBorder = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TMap<EScoreType, FLinearColor> Colors;

};
