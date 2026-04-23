// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXAssaultCourseWidget.h"
#include "Online/EXAssaultCourseGame.h"
#include "Components/TextBlock.h"

void UEXAssaultCourseWidget::Init()
{
	GM = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
	if (GM)
	{
		GM->OnScoreChanged.AddDynamic(this, &UEXAssaultCourseWidget::ScoreChanged);
		GM->OnTimeUpdated.AddDynamic(this, &UEXAssaultCourseWidget::TimeUpdated);
	}
	ScoreChanged(0);
	TimeUpdated(0);
}
void UEXAssaultCourseWidget::ScoreChanged(int32 NewScore)
{
	Score->SetText(FText::FromString(FString::FromInt(NewScore)));
}
void UEXAssaultCourseWidget::TimeUpdated(int32 DeciSeconds)
{
	const int32 Min = DeciSeconds / 600;
	const int32 Sec = (DeciSeconds % 600) / 10;
	const int32 Frac = DeciSeconds - Sec * 10;
	FText Format = FText::FromString("{0}:{1}.{2}");
	FNumberFormattingOptions DoubleDigit;
	DoubleDigit.SetMinimumIntegralDigits(2);

	FText Time = FText::Format(FTextFormat(Format), FText::AsNumber(Min, &DoubleDigit), FText::AsNumber(Sec, &DoubleDigit), FText::AsNumber(Frac));
	Timer->SetText(Time);
}
