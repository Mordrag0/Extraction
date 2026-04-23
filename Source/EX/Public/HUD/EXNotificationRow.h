// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXNotificationRow.generated.h"

class UEXNotificationList;

/**
 * 
 */
UCLASS()
class EX_API UEXNotificationRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(UEXNotificationList* InHUDOwner);

	void Remove();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	float TimeOnScreen = 2.f;

private:
	UPROPERTY()
	UEXNotificationList* HUDOwner = nullptr;

	FTimerHandle TimerHandle_Lifetime;

};
