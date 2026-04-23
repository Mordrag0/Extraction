// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXTextSelectPanel.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Player/EXPlayerController.h"


void UEXTextSelectPanel::SetSelectedCategory(const FTextPanelCategory& InCurrentCategory)
{
	CurrentCategory = InCurrentCategory;

	TArray<FStringFormatArg> StringArgs;

	TArray<FText> Options = CurrentCategory.GetTextOptions();

	int32 NumLines = FMath::Min(Options.Num(), Lines.Num());
	for (int32 Line = 0; Line < NumLines; Line++)
	{
		const FText Format = FText::FromString("{0}. {1}");
		Lines[Line]->SetText(FText::Format(Format, Line + 1 % 10, Options[Line]));
	}
	for (int32 Line = NumLines; Line < Lines.Num(); Line++)
	{
		Lines[Line]->SetText(FText::GetEmpty());
	}
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXTextSelectPanel::Close()
{
	bOpen = false;
	SetVisibility(ESlateVisibility::Hidden);
}

void UEXTextSelectPanel::NativeConstruct()
{
	Super::NativeConstruct();

	Lines.Empty();
	TArray<UWidget*> Children = TextPanel->GetAllChildren();
	for (UWidget* Child : Children)
	{
		UTextBlock* Line = Cast<UTextBlock>(Child);
		if (Line)
		{
			Lines.Emplace(Line);
		}
	}
}

