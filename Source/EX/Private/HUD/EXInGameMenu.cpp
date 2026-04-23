// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXInGameMenu.h"
#include "Components/Button.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Player/EXPlayerController.h"

void UEXInGameMenu::Open()
{
	
	LoadSettings();
	bOpen = true;
	SetVisibility(ESlateVisibility::Visible);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayer(), this, EMouseLockMode::LockOnCapture);
	GetOwningPlayer()->bShowMouseCursor = true;
}

void UEXInGameMenu::Close()
{
	if (bDirty)
	{

	}
	else
	{
		CloseWithoutSave();
	}
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer());
	GetOwningPlayer()->bShowMouseCursor = false;
}

void UEXInGameMenu::Save()
{

	bDirty = false;
}

void UEXInGameMenu::Reset()
{
	LoadSettings();
	bDirty = false;
}

void UEXInGameMenu::CloseWithoutSave()
{
	bOpen = false;
	SetVisibility(ESlateVisibility::Hidden);
}

void UEXInGameMenu::CloseWithSave()
{
	Save();
	Close();
}

void UEXInGameMenu::LoadSettings()
{

}

void UEXInGameMenu::Disconnect()
{
	Close();
	AEXPlayerController* Owner = GetOwningPlayer<AEXPlayerController>();
	Owner->Disconnect();
}

void UEXInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();

	BtnReset->OnClicked.AddDynamic(this, &UEXInGameMenu::Reset);
	BtnSave->OnClicked.AddDynamic(this, &UEXInGameMenu::Save);
	BtnResume->OnClicked.AddDynamic(this, &UEXInGameMenu::Close);
	BtnDisconnect->OnClicked.AddDynamic(this, &UEXInGameMenu::Disconnect);
}

FReply UEXInGameMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey().GetFName().IsEqual(ESCAPE_KEY))
	{
		Close();
	}

	return Reply;
}
