// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXUIMessage.h"
#include "Online/EXLocalMessage.h"
#include "GameFramework/PlayerState.h"
#include "EX.h"
#include "HUD/EXBaseChatWidget.h"
#include "HUD/EXMessageRow.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UEXUIMessage::ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject)
{
	for (int32 Idx = 0; Idx < Messages.Num() - 1; Idx++)
	{
		Messages[Idx]->CopyFrom(Messages[Idx + 1]);
	}
	Messages.Last()->ReceiveLocalMessage(MessageClass, RelatedPlayerState_1, RelatedPlayerState_2, MessageIndex, LocalMessageText, OptionalObject);
}

void UEXUIMessage::InitializeWidget(UEXBaseChatWidget* InHUDOwner)
{
	EXHUDOwner = InHUDOwner;
	for (int32 Idx = 0; Idx < RowCount; Idx++)
	{
		UEXMessageRow* Row = CreateWidget<UEXMessageRow>(this, RowClass);
		Row->SetBGColor(RowBackgroundColor);
		UVerticalBoxSlot* RowSlot = Panel->AddChildToVerticalBox(Row);
		Messages.Add(Row);
		Row->SetMessage(FText());
		Row->Hide();
	}
}

void UEXUIMessage::Show()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXUIMessage::ShowFor(float Duration)
{
	Show();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Visible, this, &UEXUIMessage::Hide, Duration, false);
}

void UEXUIMessage::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}
