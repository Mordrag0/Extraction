// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/EXInputBox.h"

void UEXInputBox::Show(bool bInTeam)
{
	bTeam = bInTeam;
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXInputBox::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}
