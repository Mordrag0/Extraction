// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXPlayerContextMenu.h"
#include "Online/EXPlayerState.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AdvancedSessionsLibrary.h"
#include "BlueprintDataDefinitions.h"
#include "AdvancedExternalUILibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Player/EXPlayerController.h"

void UEXPlayerContextMenu::Show(AEXPlayerState* InEXPS, const FVector2D Position)
{
	EXPS = InEXPS;
	SetVisibility(ESlateVisibility::Visible);
	UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (CanvasPanelSlot)
	{
		CanvasPanelSlot->SetPosition(Position);
	}
}

void UEXPlayerContextMenu::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UEXPlayerContextMenu::ViewProfile()
{
	if (EXPS)
	{
		AEXPlayerState* Owner = GetOwningPlayerState<AEXPlayerState>();
		FBPUniqueNetId OwnerId;
		UAdvancedSessionsLibrary::GetUniqueNetIDFromPlayerState(Owner, OwnerId);
		FBPUniqueNetId OtherId;
		UAdvancedSessionsLibrary::GetUniqueNetIDFromPlayerState(EXPS, OtherId);
		EBlueprintResultSwitch Result;
		UAdvancedExternalUILibrary::ShowProfileUI(OwnerId, OtherId, Result);
		if (Result == EBlueprintResultSwitch::OnSuccess)
		{
		}
	}
}

void UEXPlayerContextMenu::Report()
{
	AEXPlayerController* Owner = GetOwningPlayer<AEXPlayerController>();
	Owner->Report(EXPS);
}

void UEXPlayerContextMenu::Kick()
{

}

void UEXPlayerContextMenu::Mute()
{

}
