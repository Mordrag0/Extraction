// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXMercIcon.h"
#include "HUD/EXAugmentSelection.h"
#include "HUD/EXAugmentIcon.h"
#include "HUD/EXMercSelection.h"
#include "Components/Button.h"
#include "System/EXGameplayStatics.h"
#include "Components/Image.h"
#include "Player/EXCharacter.h"

void UEXMercIcon::Init(UEXAugmentSelection* InAugmentSelection, const FSquadMerc& InMerc)
{
	Merc = InMerc.Type;
	AugmentSelection = InAugmentSelection;

	AugmentIcons = UEXGameplayStatics::GetPanelChildren<UEXAugmentIcon>(AugmentPanel);

	int32 Idx = 0;
	for (EAugment Augment : InMerc.SelectedAugments)
	{
		AugmentIcons[Idx]->Init(Augment, Idx);
		Idx++;
	}
	OnValueChanged();
}

void UEXMercIcon::HideAugments()
{
	AugmentIcons = UEXGameplayStatics::GetPanelChildren<UEXAugmentIcon>(AugmentPanel);
	for (UEXAugmentIcon* AugmentIcon : AugmentIcons)
	{
		AugmentIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UEXMercIcon::OnValueChanged()
{
	AEXCharacter* MercCDO = UEXGameplayStatics::GetMercCDO(Merc);
	if (MercCDO)
	{
		MercIcon->SetBrushFromSoftTexture(MercCDO->GetIcon(EIconSize::Medium));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Class not loaded: %s"), *UEnum::GetValueAsString(Merc));
	}
}

void UEXMercIcon::OnAugmentClicked(UEXAugmentIcon* Icon, EAugment Augment)
{
	if (AugmentSelection)
	{
		AugmentSelection->Show(this);
	}
}

void UEXMercIcon::MercIconClicked()
{
	FSquadMerc NewMerc(Merc, GetAugmentValues());
	OnIconClicked.Broadcast(this, NewMerc);
}

void UEXMercIcon::NativeConstruct()
{
	Super::NativeConstruct();
	MercIconBtn->OnClicked.AddDynamic(this, &UEXMercIcon::MercIconClicked);

	AugmentIcons = UEXGameplayStatics::GetPanelChildren<UEXAugmentIcon>(AugmentPanel);

	for (UEXAugmentIcon* Icon : AugmentIcons)
	{
		Icon->OnIconClicked.AddDynamic(this, &UEXMercIcon::OnAugmentClicked);
	}
}

TArray<EAugment> UEXMercIcon::GetAugmentValues() const
{
	TArray<EAugment> Ret;
	for (UEXAugmentIcon* AugmentIcon : AugmentIcons)
	{
		Ret.Add(AugmentIcon->GetSelectedValue());
	}
	return Ret;
}
