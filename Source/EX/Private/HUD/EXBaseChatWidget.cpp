// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXBaseChatWidget.h"
#include "HUD/EXUIMessage.h"
#include "EX.h"
#include "Online/EXLocalMessage.h"
#include "Online/EXChatMessage.h"
#include "HUD/EXInputBox.h"
#include "Player/EXBaseController.h"
#include "Components/EditableTextBox.h"
#include "HUD/EXHUDWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UEXBaseChatWidget::ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, int32 MessageIndex, FText LocalMessageText, UObject* OptionalObject)
{
	ChatBox->ReceiveLocalMessage(MessageClass, RelatedPlayerState_1, RelatedPlayerState_2, MessageIndex, LocalMessageText, OptionalObject);
	ChatBox->ShowFor(Duration);
}

void UEXBaseChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ChatBox->InitializeWidget(this);
	InputBox->GetTextBox()->OnTextCommitted.AddDynamic(this, &UEXBaseChatWidget::Submit);
}

void UEXBaseChatWidget::Submit(const FText& Text, ETextCommit::Type CommitMethod)
{
	if ((CommitMethod == ETextCommit::OnEnter) || (CommitMethod == ETextCommit::Default))
	{
		InputBox->GetTextBox()->SetText(FText::GetEmpty());
		bool bTeam = InputBox->GetTeam();
		HUD->GetController()->Say(Text.ToString(), bTeam, false);
	}
	else if (CommitMethod == ETextCommit::OnCleared)
	{
		InputBox->GetTextBox()->SetText(FText::GetEmpty());
	}

	StopTyping(CommitMethod == ETextCommit::OnCleared);
}

void UEXBaseChatWidget::SetHUD(UEXHUDWidget* InHUD)
{
	HUD = InHUD;
}

void UEXBaseChatWidget::StartTyping(bool bTeam)
{
	bTyping = true;
	ChatBox->Show();
	InputBox->Show(bTeam);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(HUD->GetController(), InputBox->GetTextBox());
}

void UEXBaseChatWidget::StopTyping(bool bCancel)
{
	bTyping = false;
	ChatBox->ShowFor(Duration);
	InputBox->Hide();
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(HUD->GetController());
}
