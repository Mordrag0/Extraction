// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/EXTimerWidget.h"


FString UEXTimerWidget::SecondsToString() const
{
	int Mins = Seconds / 60;
	int Secs = Seconds % 60;
	FString Ret = "";
	if (Mins < 10)
	{
		Ret.Append("0");
	};
	Ret.Append(FString::FromInt(Mins));
	Ret.Append(":");
	if (Secs < 10)
	{
		Ret.Append("0");
	}
	Ret.Append(FString::FromInt(Secs));
	return Ret;
}

void UEXTimerWidget::UpdateTimer(int32 InSeconds)
{
	Seconds = InSeconds;
	OnTimerUpdated();
}
