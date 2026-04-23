// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "EXNotificationList.generated.h"

class UEXNotificationRow;
class UVerticalBox;

/**
 * Container for HUD elements that are on screen temporarily and when a new one is added all the previous ones are moved (e. g. killfeed)
 */
UCLASS()
class EX_API UEXNotificationList : public UEXHUDElement
{
	GENERATED_BODY()
	
public:
	UEXNotificationRow* AddRow();

	void RemoveNotification();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	bool bFromBottom = false;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UEXNotificationRow> RowClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	float RowTimeOnScreen = 2.f;

	UPROPERTY()
	TArray<UEXNotificationRow*> Notifications;

	UPROPERTY(VisibleAnywhere, Category = "HUD", Meta = (BindWidget))
	UVerticalBox* NotificationPanel = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	int32 MaxNotifications = 5;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	int32 RowPadding = 10;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	int32 RowSize = 30;
};
