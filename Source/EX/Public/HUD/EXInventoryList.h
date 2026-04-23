// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXInventoryList.generated.h"

class AEXMenuController;
class UEXInventoryListItem;
class UButton;

/**
 * 
 */
UCLASS()
class EX_API UEXInventoryList : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(AEXMenuController* Owner);

	void LoadItems(const TMap<uint32, uint32>& InOwnedItems);

protected:
	void Refresh();

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UEXInventoryListItem> ItemClass;

	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	class UUniformGridPanel* Grid = nullptr;

	TArray<UEXInventoryListItem*> Items;

	AEXMenuController* MenuController = nullptr;

	int32 NumGridRows = 0;
	int32 NumGridCols = 0;

	TMap<uint32, uint32> OwnedItems;

	int32 CurrentPage = 0;
	int32 NumPages = 0;

	UPROPERTY(Meta=(BindWidget))
	UButton* NextPageBtn = nullptr;
	UPROPERTY(Meta=(BindWidget))
	UButton* PreviousPageBtn = nullptr;

	UFUNCTION()
	void NextPage();
	UFUNCTION()
	void PreviousPage();

	TArray<uint32> ItemKeys;
};
