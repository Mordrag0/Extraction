// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/EXProgressWidget.h"
#include "System/EXProgress.h"
#include "Components/ProgressBar.h"


void UEXProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ProgressBar->SetFillColorAndOpacity(Color);
}

void UEXProgressWidget::UpdateProgress(float Progress)
{
	const float Percent = ProgressComp->GetRatio();
	const bool bVisibility = (Percent > 0) && (Percent < 1);
	if (bVisibility != bOldVisibility) 
	{
		ProgressBar->SetVisibility(bVisibility ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		bOldVisibility = bVisibility;
	}
	ProgressBar->SetPercent(bReversed ? 1 - Percent : Percent);
}

void UEXProgressWidget::SetProgressComponent(UEXProgress* Progress, bool bInReversed /*= false*/)
{
	ProgressComp = Progress;
	ProgressComp->ProgressUpdate.AddUObject(this, &UEXProgressWidget::UpdateProgress);
	bOldVisibility = ProgressBar->GetVisibility() != ESlateVisibility::Hidden;
	UpdateProgress(Progress->GetProgress());
	bReversed = bInReversed;
}

void UEXProgressWidget::SetColor(FLinearColor InColor)
{
	Color = InColor;
	ProgressBar->SetFillColorAndOpacity(Color);
}
