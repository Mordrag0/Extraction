// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXPrimaryObjProgress.h"
#include "Components/ProgressBar.h"
#include "System/EXProgress.h"
#include "System/EXInteract.h"
#include "Components/VerticalBox.h"

void UEXPrimaryObjProgress::SetProgress(int32 ObjectiveIdx, int32 Repeat, float Percent)
{
	int32 Idx = ObjectiveIdx + Repeat;
	if (PrimaryBars.IsValidIndex(Idx)) 
	{
		PrimaryBars[Idx]->SetPercent(Percent);
	}
}

void UEXPrimaryObjProgress::SetRepeatsCompleted(int32 ObjectiveIdx, int32 Repeat)
{
	for(int32 Idx = ObjectiveIdx; Idx < ObjectiveIdx + Repeat; Idx++)
	{
		if (PrimaryBars.IsValidIndex(Idx))
		{
			PrimaryBars[Idx]->SetPercent(1.f);
		}
	}
}

void UEXPrimaryObjProgress::SetSecondaryProgress(int32 Idx, float Percent)
{
	DefuseBars[Idx]->SetPercent(Percent);
}

void UEXPrimaryObjProgress::SetPrimaryObjectiveInfo(int32 InCount, const TArray<bool>& ShowAltBar)
{
	Count = InCount;

	for (int32 Idx = 0; Idx < PrimaryBars.Num(); Idx++)
	{
		Boxes[Idx]->SetVisibility((Count > Idx) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		PrimaryBars[Idx]->SetPercent(0.f);
		DefuseBars[Idx]->SetPercent(0.f);
		DefuseBars[Idx]->SetVisibility(((Count > Idx) && ShowAltBar[Idx]) ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
	OnNumObjectivesChanged(InCount);
}

void UEXPrimaryObjProgress::NativeConstruct()
{
	Super::NativeConstruct();

	PrimaryBars.Empty();
	PrimaryBars.Add(PrimaryProgressBar1);
	PrimaryBars.Add(PrimaryProgressBar2);
	PrimaryBars.Add(PrimaryProgressBar3);

	DefuseBars.Empty();
	DefuseBars.Add(DefuseProgressBar1);
	DefuseBars.Add(DefuseProgressBar2);
	DefuseBars.Add(DefuseProgressBar3);

	Boxes.Empty();
	Boxes.Add(Box1);
	Boxes.Add(Box2);
	Boxes.Add(Box3);
}

