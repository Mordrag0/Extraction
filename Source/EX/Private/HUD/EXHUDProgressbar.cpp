// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHUDProgressbar.h"
#include "Components/ProgressBar.h"

void UEXHUDProgressbar::SetPercent(float Ratio)
{
	Progress->SetPercent(Ratio);
}
