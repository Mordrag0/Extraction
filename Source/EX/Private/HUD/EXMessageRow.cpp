// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXMessageRow.h"
#include "Components/TextBlock.h"
#include "Online/EXLocalMessage.h"
#include "Components/Border.h"

void UEXMessageRow::ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject)
{
	const UEXLocalMessage* DefaultMessageObject = GetDefault<UEXLocalMessage>(MessageClass);
	RowText->SetText(LocalMessageText);
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if (DefaultMessageObject->Lifetime > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Visible, this, &UEXMessageRow::Hide, DefaultMessageObject->Lifetime, false);
	}
}

void UEXMessageRow::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UEXMessageRow::SetMessage(FText Message)
{
	RowText->SetText(Message);
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXMessageRow::CopyFrom(UEXMessageRow* OtherRow)
{
	RowText->SetText(OtherRow->GetText());
}

void UEXMessageRow::SetBGColor(const FLinearColor& InColor)
{
	RowBG->SetBrushColor(InColor);
}
