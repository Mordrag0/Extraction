// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXAugmentIcon.h"
#include "Components/Button.h"

void UEXAugmentIcon::Init(EAugment InType, int32 InIndex)
{
	Type = InType;
	Index = InIndex;

	OnDataSet();
}

void UEXAugmentIcon::ChangeValue(EAugment InType)
{
	Type = InType; 
	OnValueChanged();
}

void UEXAugmentIcon::OnValueChanged()
{
	OnDataSet();
}

void UEXAugmentIcon::OnClicked()
{
	OnIconClicked.Broadcast(this, Type);
}

void UEXAugmentIcon::NativeConstruct()
{
	Super::NativeConstruct();
	AugmentButton->OnClicked.AddDynamic(this, &UEXAugmentIcon::OnClicked);
}
