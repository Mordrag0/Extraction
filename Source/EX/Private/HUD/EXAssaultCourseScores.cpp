// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/EXAssaultCourseScores.h"
#include "HUD/EXAssaultCourseScoreRow.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/VerticalBox.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"

void UEXAssaultCourseScores::NativeConstruct()
{
	if (Box->GetAllChildren().Num() == 0)
	{
		for (int32 RowIdx = 0; RowIdx < NumRows; RowIdx++)
		{
			UEXAssaultCourseScoreRow* Row = CreateWidget<UEXAssaultCourseScoreRow>(this, RowClass);
			UVerticalBoxSlot* VSlot = Box->AddChildToVerticalBox(Row);
			VSlot->SetSize(FSlateChildSize());
			Rows.Add(Row);
		}
	}
}

void UEXAssaultCourseScores::ShowResult(float TotalScore, bool bPersonalBest, UTexture2D* Icon)
{
	LastScore->FillScore(TotalScore, Icon);
	if (bPersonalBest)
	{
		PersonalBest->FillScore(TotalScore, Icon);
	}
}

void UEXAssaultCourseScores::LoadScore(const FAssaultCourseStats& Scores)
{
	int32 TotalRows = FMath::Min(NumRows, Scores.Stats.Num());

	for (int32 RowIdx = 0; RowIdx < TotalRows; RowIdx++)
	{
		Rows[RowIdx]->FillScore(Scores.Stats[RowIdx]);
	}
}

void UEXAssaultCourseScores::Show()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXAssaultCourseScores::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}
