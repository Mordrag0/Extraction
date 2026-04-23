// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXMenuController.h"
#include "Player/EXPersistentUser.h"
#include "Player/EXLocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "HUD/EXMainMenu.h"
#include "Online/EXGameInstance.h"
#include "System/EXGameplayStatics.h"
#include "Player/EXCharacter.h"
#include "EXTypes.h"
#include "EX.h"
#include "Engine/AssetManager.h"
#include "Online/EXPlayerState.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "Engine/Texture2D.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "System/EXItemDataTable.h"
#include "Misc/EXMusicManager.h"

void AEXMenuController::InitInputSystem()
{
	Super::InitInputSystem();

	UEXLocalPlayer* const LocalPlayer = Cast<UEXLocalPlayer>(Player);
	UEXPersistentUser* PersistentUser = LocalPlayer ? LocalPlayer->GetPersistentUser() : nullptr;
	if (PersistentUser)
	{
		PersistentUser->TellInputAboutKeybindings();
	}
}

void AEXMenuController::BeginPlay()
{
	LoadMenuAssets();
	Super::BeginPlay();

	if (IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	bShowMouseCursor = true;
	if (MenuClass)
	{
		MainMenu = CreateWidget<UEXMainMenu>(this, MenuClass);
		MainMenu->Init(this, Cast<UEXLocalPlayer>(Player));
		MainMenu->AddToViewport();
	}

	UEXGameInstance* GI = GetWorld()->GetGameInstanceChecked<UEXGameInstance>();
	GI->MSConnect(true);
}

void AEXMenuController::LoadInventory()
{
	if (bInventoryFetched)
	{
		return;
	}
	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->RequestInventory();
	if (!InventoryIconLibrary)
	{
		InventoryIconLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
		InventoryIconLibrary->AddToRoot();
	}
	InventoryIconLibrary->LoadAssetDataFromPath(TEXT("/Game/Inventory/InventoryIconLibrary"));
	InventoryIconLibrary->LoadAssetsFromAssetData();
}

UTexture2D* AEXMenuController::GetItemIcon(uint32 ItemId)
{
	if (LoadedItemIcons.Contains(ItemId))
	{
		// Texture already loaded
		return LoadedItemIcons[ItemId];
	}

	// Load the texture, add it to the map and return it
	FEXItemData* ItemData = ItemTable->FindRow<FEXItemData>(ItemTable->GetRowNames()[ItemId - 1], FString("AEXMenuController::GetItemIcon"));

	check(ItemData);
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	UTexture2D* ItemIcon = StreamableManager.LoadSynchronous(ItemData->Icon);
	LoadedItemIcons.Add(ItemId, ItemIcon);
	return ItemIcon;
}

void AEXMenuController::LoadPlayerInventory(const TMap<uint32, uint32>& OwnedItems)
{
	if (bInventoryFetched)
	{
		return;
	}
	bInventoryFetched = true;
	MainMenu->LoadPlayerInventory(OwnedItems);
}

void AEXMenuController::LoadBasicInfo(FBasicInfo Data)
{
	SelectSquad(Data.Squad);
	MainMenu->LoadBasicInfo(Data);
}

void AEXMenuController::LoadProfileInfo(FPlayerStats PlayerStats)
{
	if (!MainMenu)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AEXMenuController::LoadProfileInfo, PlayerStats));
		return;
	}

	MainMenu->LoadProfileStats(PlayerStats);
}

void AEXMenuController::UpdateSquad(const FSquad& InSquad)
{
	SelectSquad(InSquad);

	UEXGameInstance* GI = GetWorld()->GetGameInstanceChecked<UEXGameInstance>();
	GI->UpdateSquad(InSquad);
}

void AEXMenuController::SelectSquad(const FSquad& InSquad)
{
	Squad = InSquad;
	MainMenu->SetSquad(InSquad);
}

void AEXMenuController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->MSDisconnect();
	Super::EndPlay(EndPlayReason);

	// When leaving the main menu unload menu assets
	UnloadMenuAssets();
}

void AEXMenuController::ChangeIGN(FString IGN)
{
	MainMenu->SetName(FText::FromString(IGN));

	UEXGameInstance* GI = GetWorld()->GetGameInstanceChecked<UEXGameInstance>();
	GI->ChangeIGN(IGN);
}

void AEXMenuController::ClearIGN()
{
#if WITH_EDITOR
	FString SteamName("Editor_Player");
#else
	FString SteamName = UAdvancedSteamFriendsLibrary::GetSteamPersonaName(UAdvancedSteamFriendsLibrary::GetLocalSteamIDFromSteam());
#endif
	MainMenu->SetName(FText::FromString(SteamName));

	UEXGameInstance* GI = GetWorld()->GetGameInstanceChecked<UEXGameInstance>();
	GI->ClearIGN(SteamName);
}

void AEXMenuController::ItemBought(uint32 ItemId, uint16 Count)
{
	// #EXTODO2
}

void AEXMenuController::FailedToBuy()
{
	// #EXTODO2
}

void AEXMenuController::InitPlayerState()
{
	Super::InitPlayerState();
	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	if (GI) // False in editor when not playing
	{
		GI->RequestClientInfo();
	}
}

void AEXMenuController::LoadMenuAssets()
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	// Load menu assets for all available mercs
	for (const TPair<EMerc, TSoftClassPtr<AEXCharacter>>& MercClass : GI->GetMercClasses())
	{
		// Load the merc blueprint classes
		LoadedAssets.Add(StreamableManager.LoadSynchronous(MercClass.Value));
		UE_LOG(LogEXAssets, Verbose, TEXT("Loading %s"), *MercClass.Value->GetName());

		AEXCharacter* MercCDO = UEXGameplayStatics::GetMercCDO(MercClass.Key);
		TArray<FSoftObjectPath> MercMenuAssets = MercCDO->GetMenuAssets();
		for (const FSoftObjectPath& AssetRef : MercMenuAssets)
		{
			LoadedAssets.Add(StreamableManager.LoadSynchronous(AssetRef));
			UE_LOG(LogEXAssets, Verbose, TEXT("Loading %s"), *AssetRef.GetAssetName());
		}
	}
}

void AEXMenuController::UnloadMenuAssets()
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	// Load menu assets for all available mercs
	for (const auto& MercClass : GI->GetMercClasses())
	{
		AEXCharacter* MercCDO = UEXGameplayStatics::GetMercCDO(MercClass.Key);
		TArray<FSoftObjectPath> MercMenuAssets = MercCDO->GetMenuAssets();
		for (const FSoftObjectPath& AssetRef : MercMenuAssets)
		{
			StreamableManager.Unload(AssetRef);
			UE_LOG(LogEXAssets, Verbose, TEXT("Unloading %s"), *AssetRef.GetAssetName());
		}
		// Unload the merc blueprint class
		StreamableManager.Unload(MercClass.Value.ToSoftObjectPath());
		UE_LOG(LogEXAssets, Verbose, TEXT("Unloading %s"), *MercClass.Value->GetName());
	}
	LoadedAssets.Empty();
}

