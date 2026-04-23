// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXMercSelection.h"
#include "Components/PanelWidget.h"
#include "HUD/EXMercIcon.h"
#include "Online/EXMenuController.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/PanelSlot.h"

void UEXMercSelection::Show(UEXMercIcon* InSelectedIcon)
{
	SelectedIcon = InSelectedIcon;
	AEXMenuController* Owner = GetOwningPlayer<AEXMenuController>();
	check(Owner);
	SetSquad(Owner->GetSquad());

	SetVisibility(ESlateVisibility::Visible);
}

void UEXMercSelection::Apply(UEXMercIcon* Icon, FSquadMerc Merc)
{
	AEXMenuController* Owner = GetOwningPlayer<AEXMenuController>();
	FSquad Squad = Owner->GetSquad();
	Squad.ReplaceMerc(SelectedIcon->GetMercType(), Merc.Type);
	Owner->UpdateSquad(Squad);

	SelectedIcon = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

void UEXMercSelection::SetSquad(const FSquad& Squad)
{
	MercPanel->ClearChildren(); 
	MercPanel2->ClearChildren(); 

	int32 Idx = 0;
	for (FSquadMerc Merc : Squad.Mercs)
	{
		UEXMercIcon* MercIcon = CreateWidget<UEXMercIcon>(this, MercIconClass);
		MercIcon->Init(nullptr, Merc);
		MercIcon->OnIconClicked.AddDynamic(this, &UEXMercSelection::Apply);
		MercIcon->HideAugments();

		if (Squad.SelectedMercs.Contains(Merc.Type))
		{
			// highlight?
		}

		UPanelWidget* Panel = (Idx < NumIconsInFirstRow) ? MercPanel : MercPanel2;
		UPanelSlot* PanelSlot = Panel->AddChild(MercIcon);
		UHorizontalBoxSlot* HBSlot = Cast<UHorizontalBoxSlot>(PanelSlot);
		if (HBSlot)
		{
			HBSlot->SetSize(FSlateChildSize());
		}
		Idx++;
	}
}
