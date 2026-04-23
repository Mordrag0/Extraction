// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXNotificationRow.h"
#include "HUD/EXHUDWidget.h"
#include "HUD/EXNotificationList.h"


void UEXNotificationRow::Init(UEXNotificationList* InHUDOwner)
{
	HUDOwner = InHUDOwner;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Lifetime, this, &UEXNotificationRow::Remove, TimeOnScreen);
}

void UEXNotificationRow::Remove()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Lifetime);
	SetVisibility(ESlateVisibility::Hidden);
	if (HUDOwner)
	{
		HUDOwner->RemoveNotification();
	}
	RemoveFromParent();
}
