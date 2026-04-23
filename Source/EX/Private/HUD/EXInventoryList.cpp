// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXInventoryList.h"
#include "HUD/EXInventoryListItem.h"
#include "Engine/Texture2D.h"
#include "Online/EXMenuController.h"
#include "System/EXGameplayStatics.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"

void UEXInventoryList::Init(AEXMenuController* Owner)
{
	MenuController = Owner;
	Items = UEXGameplayStatics::GetPanelChildren<UEXInventoryListItem>(Grid);
	for (UEXInventoryListItem* ListItem : Items)
	{
		ListItem->Init(this);
		UUniformGridSlot* GridSlot = Cast<UUniformGridSlot>(ListItem->Slot);
		if (GridSlot)
		{
			NumGridRows = FMath::Max(NumGridRows, GridSlot->GetRow() + 1);
			NumGridCols = FMath::Max(NumGridCols, GridSlot->GetColumn() + 1);
			GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
	}

	NextPageBtn->OnClicked.AddDynamic(this, &UEXInventoryList::NextPage);
	PreviousPageBtn->OnClicked.AddDynamic(this, &UEXInventoryList::PreviousPage);
}

void UEXInventoryList::LoadItems(const TMap<uint32, uint32>& InOwnedItems)
{
	OwnedItems = InOwnedItems;
	InOwnedItems.GetKeys(ItemKeys);
	CurrentPage = 1;
	const uint32 NumItemsPerPage = NumGridRows * NumGridCols;
	NumPages = (InOwnedItems.Num() - 1) / NumItemsPerPage + 1;

	Refresh();
}

void UEXInventoryList::Refresh()
{
	if (ItemKeys.Num() == 0)
	{
		const uint32 NumItemsPerPage = NumGridRows * NumGridCols;
		for (uint32 ItemIdx = 0; ItemIdx < NumItemsPerPage; ItemIdx++)
		{
			Items[ItemIdx]->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}
	const uint32 NumItemsPerPage = NumGridRows * NumGridCols;
	const uint32 From = (CurrentPage - 1) * NumItemsPerPage;
	const uint32 To = FMath::Min(CurrentPage * NumItemsPerPage, (uint32)ItemKeys.Num()) - 1;
	for (uint32 ItemIdx = From; ItemIdx <= To; ItemIdx++)
	{
		const uint32 ItemId = ItemKeys[ItemIdx];
		UTexture2D* ItemTexture = MenuController->GetItemIcon(ItemId);
		Items[ItemIdx]->SetItem(ItemId, OwnedItems[ItemId], ItemTexture);
		Items[ItemIdx]->SetVisibility(ESlateVisibility::Visible);
	}
	for (uint32 ItemIdx = To + 1; ItemIdx < CurrentPage * NumItemsPerPage; ItemIdx++)
	{
		Items[ItemIdx]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEXInventoryList::NativeConstruct()
{
	Super::NativeConstruct();

	check(ItemClass);
	Items = UEXGameplayStatics::GetPanelChildren<UEXInventoryListItem>(Grid);
}

void UEXInventoryList::NextPage()
{
	CurrentPage = FMath::Min(CurrentPage + 1, NumPages);
	Refresh();
}

void UEXInventoryList::PreviousPage()
{
	CurrentPage = FMath::Max(CurrentPage - 1, 1);
	Refresh();
}
