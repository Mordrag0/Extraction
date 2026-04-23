// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXNotificationList.h"
#include "HUD/EXNotificationRow.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

UEXNotificationRow* UEXNotificationList::AddRow()
{
	check(RowClass && "UI class not set");
	if (!RowClass)
	{
		return nullptr;
	}

	UEXNotificationRow* Notification = CreateWidget<UEXNotificationRow>(this, RowClass);
	Notification->Init(this);
	if (Notifications.Num() == MaxNotifications)
	{
		Notifications[0]->Remove();
	}
	UVerticalBoxSlot* NewRowSlot = NotificationPanel->AddChildToVerticalBox(Notification);
	NewRowSlot->SetPadding(FMargin(0, RowPadding, 0, RowPadding));
	int32 Idx = Notifications.Num();
	Notifications.Add(Notification);
	
	return Notification;
}

void UEXNotificationList::RemoveNotification()
{
	Notifications.RemoveAt(0);
}
