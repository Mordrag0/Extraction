// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXDeathScreen.h"
#include "HUD/EXMercSelectIcon.h"
#include "Components/TextBlock.h"
#include "Player/EXCharacter.h"

void UEXDeathScreen::MercSelectionChanged(int32 MercIdx)
{
	if (SelectedMerc == MercIdx)
	{
		return;
	}
	UEXMercSelectIcon* OldSelection = GetMercIcon(SelectedMerc);
	OldSelection->Unhighlight();
	UEXMercSelectIcon* NewSelection = GetMercIcon(MercIdx);
	NewSelection->Highlight();
	SelectedMerc = MercIdx;
}

void UEXDeathScreen::SetCanRespawn(bool bCanRespawn)
{
	RespawnMessage->SetVisibility(bCanRespawn ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UEXDeathScreen::SetIcons(const TArray<TSoftClassPtr<AEXCharacter>>& PlayerClasses)
{
	if (PlayerClasses.Num() >= 3)
	{
		AEXCharacter* PrimaryCDO = PlayerClasses[0]->GetDefaultObject<AEXCharacter>();
		if (PrimaryCDO)
		{
			PrimaryMerc->SetIcon(PrimaryCDO->GetIcon(EIconSize::Medium));
		}
		AEXCharacter* SecondaryCDO = PlayerClasses[1]->GetDefaultObject<AEXCharacter>();
		if (SecondaryCDO)
		{
			SecondaryMerc->SetIcon(SecondaryCDO->GetIcon(EIconSize::Medium));
		}
		AEXCharacter* TertiaryCDO = PlayerClasses[2]->GetDefaultObject<AEXCharacter>();
		if (TertiaryCDO)
		{
			TertiaryMerc->SetIcon(TertiaryCDO->GetIcon(EIconSize::Medium));
		}
	}
}

void UEXDeathScreen::Show()
{
	if (bSpec)
	{
		Hide();
	}
	else
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UEXDeathScreen::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

UEXMercSelectIcon* UEXDeathScreen::GetMercIcon(int32 Idx) const
{
	UEXMercSelectIcon* MercPanel = nullptr;
	switch (Idx)
	{
		case 0: MercPanel = PrimaryMerc; break;
		case 1: MercPanel = SecondaryMerc; break;
		case 2: MercPanel = TertiaryMerc; break;
		default:
			break;
	}
	return MercPanel;
}

void UEXDeathScreen::NativeConstruct()
{
	Super::NativeConstruct();

	SecondaryMerc->Unhighlight();
	TertiaryMerc->Unhighlight();
}
