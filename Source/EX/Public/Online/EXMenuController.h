// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/EXBaseController.h"
#include "EXMenuController.generated.h"

class UEXMainMenu;
class AEXCharacter;
class UEXLocalPlayer;
class UDataTable;

/**
 * 
 */
UCLASS()
class EX_API AEXMenuController : public AEXBaseController
{
	GENERATED_BODY()
	
public:

	virtual void InitInputSystem() override;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	TSubclassOf<UEXMainMenu> MenuClass = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Menu")
	UEXMainMenu* GetMainMenuWidget() const { return MainMenu; }

	void LoadBasicInfo(FBasicInfo Data);
	void LoadProfileInfo(FPlayerStats PlayerStats);

	void UpdateSquad(const FSquad& InSquad);
	void SelectSquad(const FSquad& InSquad);


	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FSquad GetSquad() const { return Squad; }

	void ChangeIGN(FString IGN);

	void ClearIGN();

	void ItemBought(uint32 ItemId, uint16 Count);
	void FailedToBuy();


	virtual void InitPlayerState() override;

	void LoadInventory();

	UTexture2D* GetItemIcon(uint32 ItemId);

	void LoadPlayerInventory(const TMap<uint32, uint32>& OwnedItems);

	void RefreshVolume();
protected:
	UPROPERTY()
	TArray<UObject*> LoadedAssets;

	void LoadMenuAssets();
	void UnloadMenuAssets();

	virtual void BeginPlay() override;

	FSquad Squad;

	UPROPERTY(BlueprintReadOnly, Category = "Menu")
	UEXMainMenu* MainMenu = nullptr;

	UPROPERTY()
	class UObjectLibrary* InventoryIconLibrary = nullptr;

	UPROPERTY()
	TMap<uint32, UTexture2D*> LoadedItemIcons;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	UDataTable* ItemTable = nullptr;

	bool bInventoryFetched = false;

};
