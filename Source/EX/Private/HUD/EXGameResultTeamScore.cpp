// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXGameResultTeamScore.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "System/EXGameplayStatics.h"
#include "Components/TextBlock.h"
#include "Math/UnrealMathUtility.h"

void UEXGameResultTeamScore::Init(const FRoundScore& Score)
{
	TArray<UHorizontalBox*> Rows = UEXGameplayStatics::GetPanelChildren<UHorizontalBox>(Root);
	int32 NumObjectives = Score.ObjectiveScores.Num();
	int32 NumRows = FMath::Min(Rows.Num(), NumObjectives);
	for (int32 Row = 0; Row < NumRows; Row++)
	{
		TArray<UTextBlock*> Blocks = UEXGameplayStatics::GetPanelChildren<UTextBlock>(Rows[Row]);
		int32 NumCols = FMath::Min(Blocks.Num(), Score.ObjectiveScores[Row].Num());
		for (int32 Col = 0; Col < NumCols; Col++)
		{
			Blocks[Col]->SetText(UEXGameplayStatics::FloatToText(Score.ObjectiveScores[Row][Col], 1));
		}
		for (int32 Col = NumCols; Col < Blocks.Num(); Col++)
		{
			Blocks[Col]->SetText(FText());
		}
	}
	for (int32 Row = NumRows; Row < Rows.Num(); Row++)
	{
		TArray<UTextBlock*> Blocks = UEXGameplayStatics::GetPanelChildren<UTextBlock>(Rows[Row]);
		for (int32 Col = 0; Col < Blocks.Num(); Col++)
		{
			Blocks[Col]->SetText(FText());
		}
	}
}
