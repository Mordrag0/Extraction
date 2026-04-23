// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXSquad.h"
#include "EXTypes.h"
#include "EX.h"
#include "HUD/EXMercIcon.h"
#include "System/EXGameplayStatics.h"
#include "HUD/EXMercSelection.h"

void UEXSquad::SetSquad(const FSquad& Squad)
{
	check((Squad.SelectedMercs.Num() == MercIcons.Num()) && "Number of mercs in squad doesn't match with number of merc icons.");
	int32 MercIdx = 0;
	for (EMerc Merc : Squad.SelectedMercs)
	{
		MercIcons[MercIdx]->Init(AugmentSelection, Squad.GetMerc(Merc));
		MercIdx++;
	}
}

void UEXSquad::ChangeMerc(UEXMercIcon* Icon, FSquadMerc Merc)
{
	MercSelection->Show(Icon);
}

void UEXSquad::NativeConstruct()
{
	Super::NativeConstruct();

	MercIcons = UEXGameplayStatics::GetPanelChildren<UEXMercIcon>(MercIconPanel);

	for (UEXMercIcon* Icon : MercIcons)
	{
		Icon->OnIconClicked.AddDynamic(this, &UEXSquad::ChangeMerc);
	}
}
