// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXAugmentSelection.h"
#include "Components/PanelWidget.h"
#include "HUD/EXAugmentIcon.h"
#include "Player/EXCharacter.h"
#include "System/EXGameplayStatics.h"
#include "HUD/EXMercIcon.h"
#include "Components/Button.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/PanelSlot.h"
#include "Online/EXMenuController.h"
#include "Online/EXPlayerState.h"
#include "EXTypes.h"

void UEXAugmentSelection::Show(UEXMercIcon* InMercIcon)
{
	MercIcon = InMercIcon;
	Init(MercIcon->GetMercType());
	SetVisibility(ESlateVisibility::Visible);
}

void UEXAugmentSelection::Apply()
{
	if (SelectedAugments.Num() != 3)
	{
		return;
	}
	AEXMenuController* Owner = GetOwningPlayer<AEXMenuController>();
	FSquad Squad = Owner->GetSquad();
	Squad.SwapAugments(MercIcon->GetMercType(), SelectedAugments);
	Owner->UpdateSquad(Squad);

	SetVisibility(ESlateVisibility::Hidden);
	MercIcon = nullptr;
}

void UEXAugmentSelection::Init(EMerc Merc)
{
	ButtonPanel->ClearChildren();

	AEXCharacter* CharCDO = UEXGameplayStatics::GetMercCDO(Merc);
	TArray<EAugment> AvailableAugments = CharCDO->GetAvailableAugments();

	SelectedAugments = GetOwningPlayer<AEXMenuController>()->GetSquad().GetMerc(Merc).SelectedAugments;

	int32 Idx = 0;
	for (EAugment Augment : AvailableAugments)
	{
		UEXAugmentIcon* AugmentIcon = CreateWidget<UEXAugmentIcon>(this, AugmentIconClass);
		AugmentIcon->Init(Augment, Idx++);
		AugmentIcon->OnIconClicked.AddDynamic(this, &UEXAugmentSelection::AugmentClicked);
		if (SelectedAugments.Contains(Augment))
		{
			AugmentIcon->SetHighlighted(true);
		}

		UPanelSlot* PanelSlot = ButtonPanel->AddChild(AugmentIcon);
		UHorizontalBoxSlot* HBSlot = Cast<UHorizontalBoxSlot>(PanelSlot);
		if (HBSlot)
		{
			HBSlot->SetSize(FSlateChildSize());
		}
	}
}

void UEXAugmentSelection::AugmentClicked(UEXAugmentIcon* Icon, EAugment Augment)
{
	if (SelectedAugments.Contains(Augment))
	{
		SelectedAugments.Remove(Augment);
		Icon->SetHighlighted(false);
	}
	else
	{
		if (SelectedAugments.Num() < MaxSelectedAugments)
		{
			SelectedAugments.Add(Augment);
			Icon->SetHighlighted(true);
		}
	}
}

void UEXAugmentSelection::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnApply)
	{
		BtnApply->OnClicked.AddDynamic(this, &UEXAugmentSelection::Apply);
	}
}
