// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXLoadingMap.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXLoadingMap : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Show(const FString& Map);

	UFUNCTION(BlueprintImplementableEvent, Category = "Map")
	void OnBeginMapLoad(const FString& Map);
};
