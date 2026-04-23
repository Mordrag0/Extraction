// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXProfileStats.h"
#include "Components/UniformGridPanel.h"

void UEXProfileStats::SetData(const FPlayerStats& PlayerStats)
{

}

void UEXProfileStats::NativeConstruct()
{
	Super::NativeConstruct();

	/*Mercs.Empty(); 

	int32 MercCount = MercsPanel->GetChildrenCount();
	for (int32 Idx = 0; Idx < MercCount; Idx++)
	{
		UEXMercStats* Merc = Cast<UEXMercStats>(MercsPanel->GetChildAt(Idx));
		if (Merc)
		{
			Mercs.Add(Merc);
		}
	}*/
}

void UEXProfileStats::LoadStats(const FPlayerStats& PlayerStats)
{
}
