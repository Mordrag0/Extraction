// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXOptionsInputRow.h"
#include "HUD/EXOptionsInput.h"
#include "Components/TextBlock.h"
#include "Components/InputKeySelector.h"

void UEXOptionsInputRow::Init(UEXOptionsInput* Owner)
{
	OwnerWidget = Owner;
}

void UEXOptionsInputRow::Fill(const FName& InBindName, const FName& InDisplayName, bool bIsInverted, const FKey& Input)
{
	Selector->OnKeySelected.RemoveDynamic(this, &UEXOptionsInputRow::KeyChanged);
	BindName = InBindName;
	DisplayName = InDisplayName;
	bInverted = bIsInverted;
	BindText->SetText(FText::FromName(DisplayName));
	Selector->SetSelectedKey(FInputChord(Input));
	Selector->OnKeySelected.AddDynamic(this, &UEXOptionsInputRow::KeyChanged);
}

FKey UEXOptionsInputRow::GetKey() const
{
	return Selector->SelectedKey.Key;
}

UInputKeySelector* UEXOptionsInputRow::GetSelector()
{
	return Selector;
}

void UEXOptionsInputRow::KeyChanged(FInputChord SelectedKey)
{
	OwnerWidget->InputChanged(FMyInputId(BindName, bInverted), SelectedKey.Key);
}
