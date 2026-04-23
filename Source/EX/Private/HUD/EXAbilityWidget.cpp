// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXAbilityWidget.h"
#include "Inventory/EXInventory.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UEXAbilityWidget::InitializeHUD(UEXInventory* Ability)
{
	Icon->SetBrushFromSoftTexture(Ability->GetIcon(EIconSize::Small));
	DisplayName->SetText(FText::FromName(Ability->GetDisplayName()));
	RecoverProgress->SetPercent(1.f);
}

void UEXAbilityWidget::SetPercent(float Value)
{
	RecoverProgress->SetPercent(Value);
}
