// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXMenuError.h"
#include "Components/Button.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Components/TextBlock.h"

void UEXMenuError::Error(FText Message, bool bFatalError)
{
	if (bOpen && bFatal)
	{
		return;
	}
	bOpen = true;
	bFatal = bFatalError;
	SetVisibility(ESlateVisibility::Visible);
	MessageBlock->SetText(Message);
	CloseBtn->SetVisibility(ESlateVisibility::Visible);
}

void UEXMenuError::Close()
{
	if (bFatal)
	{
		FGenericPlatformMisc::RequestExit(false);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	bOpen = false;
}

void UEXMenuError::NativeConstruct()
{
	Super::NativeConstruct();

	CloseBtn->OnClicked.AddDynamic(this, &UEXMenuError::Close);
}
