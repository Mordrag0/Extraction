// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXGameResultPlayer.generated.h"

class UPanelWidget;

/**
 * 
 */
UCLASS()
class EX_API UEXGameResultPlayer : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(const FPlayerStats& Result);
	void Clear();
protected:

	bool bInitialized = false;

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* Root = nullptr;
};
