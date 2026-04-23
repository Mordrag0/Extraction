// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXInventoryListItem.h"
#include "Engine/Texture2D.h"
#include "HUD/EXInventoryList.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UEXInventoryListItem::Init(UEXInventoryList* Owner)
{
	InventoryOwner = Owner;
	ItemButton->OnClicked.AddDynamic(this, &UEXInventoryListItem::OnClicked);
}

void UEXInventoryListItem::SetItem(uint32 ItemId, uint32 ItemCount, UTexture2D* ItemTexture)
{
	ItemImage->SetBrushFromTexture(ItemTexture);
	Count->SetText(FText::FromString(FString::FromInt(ItemCount)));
	Id = ItemId;
}

void UEXInventoryListItem::OnClicked()
{
}
