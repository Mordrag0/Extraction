// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "FindSessionsCallbackProxy.h"
#include "EXMainMenu.generated.h"

class UEXLocalPlayer;
class UTextBlock;
class AEXMenuController;
class UEXSquad;
class UEditableText;
class UEXOldGames;
class UEXMenuError;
class UEXInventoryList;
class UEXMainMenuInner;

/**
 * 
 */
UCLASS()
class EX_API UEXMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(AEXMenuController* InMC, UEXLocalPlayer* InLP);

	UFUNCTION()
	void OptionsClosed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Menu")
	void OnOptionsClosed();

	void LoadBasicInfo(const FBasicInfo& Data);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetName(FText Name);
	UFUNCTION(BlueprintCallable, Category = "Data")
	void ChangeName(FText Name);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void GetServerInfo(const struct FBlueprintSessionResult& SearchResult);

	void SetSquad(const FSquad& InSquad);
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnFirstTimeLogIn();
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OfflineMessage();

	void LoadProfileStats(const FPlayerStats& PlayerStats);
	void HideLoadingScreen();
	void LoadOldGames(const TArray<FGameInfo>& OldGames);
	void LoadOldGame(uint64 GameId, const FGameResultSW& OldGame);
	void OutdatedClient();
	void CurrentlyOffline();
	void MSTimeout();

	void LoadStoreItems(const TArray<FShopItem>& ShopItems);
	void LoadPlayerInventory(const TMap<uint32, uint32>& OwnedItems);
protected:
	UFUNCTION(BlueprintCallable, Category = "Data")
	void RequestProfileStats();
	UFUNCTION(BlueprintCallable, Category = "Data")
	void RequestOldGames();
	UFUNCTION(BlueprintCallable, Category = "Data")
	void LoadInventory();

	UPROPERTY()
	class AEXOnlineBeaconClient* BeaconClient = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Menu", Meta = (BindWidget))
	class UEXOptions* Options = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI", Meta = (BindWidget))
	UEXSquad* SquadUI = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "UI", Meta = (BindWidget))
	class UEXProfileStats* Profile = nullptr;

	UEXLocalPlayer* LP = nullptr;
	AEXMenuController* MenuController = nullptr;

	FText CurrentName;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UUserWidget* WBP_Loading = nullptr;
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UEXMenuError* ClientMessageScreen = nullptr;
	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly, Category = "Menu", Meta = (BindWidget))
	UEXOldGames* WBP_OldGames = nullptr;
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UEXInventoryList* WBP_InventoryList = nullptr;
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UEXMainMenuInner* WBP_MainInner = nullptr;

	bool bOldGamesLoaded = false;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FText ExpTextFormat = NSLOCTEXT("UEXMainMenu", "ExpText", "Exp: {Value}");
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FText CreditsTextFormat = NSLOCTEXT("UEXMainMenu", "CreditsText", "Credits: {Value}");
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FText NoSteamFormat = NSLOCTEXT("UEXMainMenu", "NoSteam", "Could not establish connection to Steam.");
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FText OutDatedClientFormat = NSLOCTEXT("UEXMainMenu", "OutDatedClient", "Your client is outdated. Please update to the latest version.");
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FText CurrentlyOfflineFormat = NSLOCTEXT("UEXMainMenu", "CurrentlyOffline", "Game is currently offline.");
	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	FText ConnectionTimeoutFormat = NSLOCTEXT("UEXMainMenu", "ConnectionTimeout", "Connection timed out.");

};
