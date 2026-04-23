// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXInventoryListItem.generated.h"

class UTexture2D;
class UButton;
class AEXMenuController;
class UImage;
class UTextBlock;
class UEXInventoryList;

/**
 * 
 */
UCLASS()
class EX_API UEXInventoryListItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(UEXInventoryList* Owner);
	void SetItem(uint32 ItemId, uint32 ItemCount, UTexture2D* ItemTexture);
protected:
	UFUNCTION()
	void OnClicked();

	UPROPERTY(Meta = (BindWidget))
	UImage* ItemImage = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* ItemButton = nullptr;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Count = nullptr;

	uint32 Id = 0;

	class UEXInventoryList* InventoryOwner = nullptr;
};
