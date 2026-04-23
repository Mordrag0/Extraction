// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ExMainMenuInner.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h"
#include "HUD/EXMainMenu.h"

void UEXMainMenuInner::Init(UEXMainMenu* InOwner, uint64 Exp, uint64 Credits)
{
	Owner = InOwner;
	{
		FFormatNamedArguments Args;
		Args.Add(FString("Value"), Exp);
		ExpText->SetText(FText::Format(FTextFormat(Owner->ExpTextFormat), Args));
	}
	{
		FFormatNamedArguments Args;
		Args.Add(FString("Value"), Credits);
		CreditsText->SetText(FText::Format(FTextFormat(Owner->CreditsTextFormat), Args));
	}
}

void UEXMainMenuInner::SetName(const FText& InName)
{
	NameText->SetText(InName);
}
