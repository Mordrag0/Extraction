// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXMercStatusIcon.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Engine/Texture2D.h"
#include "EXTypes.h"

void UEXMercStatusIcon::SetDefaultTexture()
{
	Icon->SetBrushFromTexture(DefaultTexture);
}

void UEXMercStatusIcon::SetTexture(TSoftObjectPtr<UTexture2D> CharacterIcon)
{
	Icon->SetBrushFromSoftTexture(CharacterIcon);
}

void UEXMercStatusIcon::SetStatus(EMercStatus Status)
{
	switch (Status)
	{
		case EMercStatus::Alive:
			UE_LOG(LogTemp, Warning, TEXT("SetStatus Alive"));
			SetVisibility(ESlateVisibility::HitTestInvisible);
			Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
			DeathEffect->SetVisibility(ESlateVisibility::Hidden);
			GibbedEffect->SetVisibility(ESlateVisibility::Hidden);
			break;
		case EMercStatus::Downed:
			UE_LOG(LogTemp, Warning, TEXT("SetStatus Downed"));
			SetVisibility(ESlateVisibility::HitTestInvisible);
			Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
			DeathEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
			GibbedEffect->SetVisibility(ESlateVisibility::Hidden);
			break;
		case EMercStatus::Gibbed:
			UE_LOG(LogTemp, Warning, TEXT("SetStatus Gibbed"));
			SetVisibility(ESlateVisibility::HitTestInvisible);
			Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
			DeathEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
			GibbedEffect->SetVisibility(ESlateVisibility::HitTestInvisible);
			break;
		case EMercStatus::Unknown:
			UE_LOG(LogTemp, Warning, TEXT("SetStatus Unknown"));
			SetVisibility(ESlateVisibility::HitTestInvisible);
			Icon->SetVisibility(ESlateVisibility::Hidden);
			DeathEffect->SetVisibility(ESlateVisibility::Hidden);
			GibbedEffect->SetVisibility(ESlateVisibility::Hidden);
			break;
		case EMercStatus::None:
		default:
			UE_LOG(LogTemp, Warning, TEXT("SetStatus None"));
			SetVisibility(ESlateVisibility::Collapsed);
			Icon->SetVisibility(ESlateVisibility::Hidden);
			DeathEffect->SetVisibility(ESlateVisibility::Hidden);
			GibbedEffect->SetVisibility(ESlateVisibility::Hidden);
			break;
	}
}

void UEXMercStatusIcon::SetTeamColor(bool bFriendly)
{
}

