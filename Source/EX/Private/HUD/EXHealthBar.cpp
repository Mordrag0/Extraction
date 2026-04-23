// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHealthBar.h"
#include "Components/ProgressBar.h"

void UEXHealthBar::SetHealth(float InPercent)
{
	HealthProgressbar->SetPercent(InPercent);
}

void UEXHealthBar::Show()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXHealthBar::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

