// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXMainMenu.h"
#include "HUD/EXOptions.h"
#include "Player/EXLocalPlayer.h"
#include "Components/TextBlock.h"
#include "Online/EXOnlineBeaconClient.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/EXMenuController.h"
#include "HUD/EXSquad.h"
#include "Components/EditableText.h"
#include "GameFramework/PlayerState.h"
#include "EX.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "HUD/EXProfileStats.h"
#include "Online/EXGameInstance.h"
#include "Online/EXPlayerState.h"
#include "System/EXGameplayStatics.h"
#include "HUD/EXOldGames.h"
#include "HUD/EXMenuError.h"
#include "HUD/EXInventoryList.h"
#include "HUD/EXMainMenuInner.h"

void UEXMainMenu::Init(AEXMenuController* InMC, UEXLocalPlayer* InLP)
{
	MenuController = InMC;
	LP = InLP;
	Options->Init(LP);
	Options->OnClosed.BindUObject(this, &UEXMainMenu::OptionsClosed);

	if (!UEXGameplayStatics::SteamOnline())
	{
		ClientMessageScreen->Error(NoSteamFormat, true);
	}
	WBP_InventoryList->Init(InMC);
}

void UEXMainMenu::OptionsClosed()
{
	OnOptionsClosed();
}

void UEXMainMenu::LoadBasicInfo(const FBasicInfo& Data)
{
	WBP_MainInner->Init(this, Data.Exp, Data.Credits);
	if (!Data.Name.IsNone()) // Custom name
	{
		SetName(FText::FromName(Data.Name));
	}
	else
	{
		SetName(FText::FromString(GetOwningPlayer()->GetPlayerState<APlayerState>()->GetPlayerName()));
	}
	if (Data.bNewlyCreated) // First time player
	{
		OnFirstTimeLogIn();
	}
	WBP_Loading->SetVisibility(ESlateVisibility::Hidden);
}

void UEXMainMenu::SetName(FText Name)
{
	WBP_MainInner->SetName(Name);
	CurrentName = Name;
}

void UEXMainMenu::ChangeName(FText Name)
{
	if (Name.EqualTo(CurrentName))
	{
		return;
	}
	AEXMenuController* PC = GetOwningPlayer<AEXMenuController>();
	if (!Name.IsEmpty())
	{
		PC->ChangeIGN(Name.ToString());
	}
	else
	{
		PC->ClearIGN();
	}
}

void UEXMainMenu::GetServerInfo(const FBlueprintSessionResult& SearchResult)
{
	if (BeaconClient)
	{
		BeaconClient->Disconnect();
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwningPlayer();
	BeaconClient = GetWorld()->SpawnActor<AEXOnlineBeaconClient>(AEXOnlineBeaconClient::StaticClass(), FTransform(), SpawnParams);

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	IOnlineSessionPtr Sessions = nullptr;
	if (OnlineSub)
	{
		Sessions = OnlineSub->GetSessionInterface();
		FString ConnectInfo;
		Sessions->GetResolvedConnectString(SearchResult.OnlineResult, NAME_BeaconPort, ConnectInfo);
		if (BeaconClient->Start(ConnectInfo))
		{
			BeaconClient->ServerPong();
		}
	}

}

void UEXMainMenu::SetSquad(const FSquad& InSquad)
{
	SquadUI->SetSquad(InSquad);
}

void UEXMainMenu::RequestProfileStats()
{
	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->RequestProfileInfo();
}

void UEXMainMenu::RequestOldGames()
{
	if (bOldGamesLoaded)
	{
		return;
	}
	UEXGameInstance* GI = GetWorld()->GetGameInstance<UEXGameInstance>();
	GI->RequestOldGames();
}

void UEXMainMenu::LoadInventory()
{
	MenuController->LoadInventory();
}

void UEXMainMenu::LoadProfileStats(const FPlayerStats& PlayerStats)
{
	Profile->LoadStats(PlayerStats);
}

void UEXMainMenu::HideLoadingScreen()
{
	WBP_Loading->SetVisibility(ESlateVisibility::Hidden);
	SetName(FText::FromString(FString::Printf(TEXT("Offline"))));
}

void UEXMainMenu::LoadOldGames(const TArray<FGameInfo>& OldGames)
{
	if (bOldGamesLoaded)
	{
		return;
	}
	bOldGamesLoaded = true;
	WBP_OldGames->Init(OldGames);
}

void UEXMainMenu::LoadOldGame(uint64 GameId, const FGameResultSW& OldGame)
{
	WBP_OldGames->ReceiveGameInfo(GameId, OldGame);
}

void UEXMainMenu::OutdatedClient()
{		
	ClientMessageScreen->Error(OutDatedClientFormat, true);
}

void UEXMainMenu::CurrentlyOffline()
{
	ClientMessageScreen->Error(CurrentlyOfflineFormat, true);
}

void UEXMainMenu::MSTimeout()
{
	ClientMessageScreen->Error(ConnectionTimeoutFormat, true);
}

void UEXMainMenu::LoadStoreItems(const TArray<FShopItem>& ShopItems)
{

}

void UEXMainMenu::LoadPlayerInventory(const TMap<uint32, uint32>& OwnedItems)
{
	WBP_InventoryList->LoadItems(OwnedItems); 
}

