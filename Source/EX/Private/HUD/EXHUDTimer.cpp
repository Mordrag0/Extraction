// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHUDTimer.h"
#include "Components/TextBlock.h"

void UEXHUDTimer::SetText(FText Text)
{
	TimerText->SetText(Text);
}

void UEXHUDTimer::DeleteText()
{
	TimerText->SetText(FText());
}
