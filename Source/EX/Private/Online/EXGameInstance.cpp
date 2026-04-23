// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameInstance.h"
#include "Engine/StreamableManager.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/SoftObjectPath.h"
#include "Online/EXGameSession.h"
#include "GameFramework/GameModeBase.h"
#include "Online/EXMenuController.h"
#include "LowEntryExtendedStandardLibrary.h"
#include "System/EXGameplayStatics.h"
#include "EX.h"
#include "Player/EXPlayerController.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "Player/EXBaseController.h"
#include "Online/EXPlayerState.h"
#include "Online/EXLevelRules.h"
#include "Online/EXAssaultCourseGame.h"
#include "HUD/EXMainMenu.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "Online/EXGameModeLobby.h"
#include "Online/EXGameModeBase.h"
#include "HUD/EXLoadingMap.h"
#include "Engine/AssetManager.h"


namespace EXGameInstanceState
{
	const FName None = FName(TEXT("None"));
	const FName PendingInvite = FName(TEXT("PendingInvite"));
	const FName WelcomeScreen = FName(TEXT("WelcomeScreen"));
	const FName MainMenu = FName(TEXT("MainMenu"));
	const FName MessageMenu = FName(TEXT("MessageMenu"));
	const FName Playing = FName(TEXT("Playing"));
}

void UEXGameInstance::Init()
{
	Super::Init();

	if(!IsRunningDedicatedServer())
	{
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UEXGameInstance::BeginLoadingScreen);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UEXGameInstance::EndLoadingScreen);
		if(TravelWidgetClass)
		{
			TravelWidget = CreateWidget<UUserWidget>(this, TravelWidgetClass);
		}
		if (MapLoadWidgetClass)
		{
			MapLoadWidget = CreateWidget<UEXLoadingMap>(this, MapLoadWidgetClass);
		}
		FOnPreClientTravel& OnPreClientTravelDelegate = OnNotifyPreClientTravel();
		OnPreClientTravelDelegate.AddUObject(this, &UEXGameInstance::PreClientTravel);
		
	}

	DisconnectedDelegate.BindDynamic(this, &UEXGameInstance::ONMSDisconnected);
	ConnectedDelegate.BindDynamic(this, &UEXGameInstance::ONMSConnected);
	MessageReceivedDelegate.BindDynamic(this, &UEXGameInstance::ONMSMessageReceived);
}

void UEXGameInstance::Shutdown()
{
	MSDisconnect();

	Super::Shutdown();
}

AEXGameSession* UEXGameInstance::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AGameModeBase* const Game = World->GetAuthGameMode();
		if (Game)
		{
			return Cast<AEXGameSession>(Game->GameSession);
		}
	}

	return nullptr;
}

void UEXGameInstance::RequestClientInfo()
{
	if (IsDedicatedServerInstance())
	{
		ensure(0);
		UE_LOG(LogEXOnline, Error, TEXT("Requesting client info on a dedicated server"));
		return;
	}
	MSSendData(FEXSerialize(EMSRequest::CLIENT_INFO, FString(CLIENT_VERSION), UEXGameplayStatics::GetLocalSteamID(this), UEXGameplayStatics::GetSteamName(this)));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MSTimeout, this, &UEXGameInstance::Timeout, MSTimeoutTime, false);
}

void UEXGameInstance::RequestPlayerInfo(AEXPlayerState* PS)
{
	MSSendData(FEXSerialize(EMSRequest::PLAYER_INFO, PS->GetSteamID()));
}

void UEXGameInstance::RequestProfileInfo()
{
	MSSendData(FEXSerialize(EMSRequest::PROFILE_INFO, UEXGameplayStatics::GetLocalSteamID(this)));
}

void UEXGameInstance::RequestOldGames()
{
	MSSendData(FEXSerialize(EMSRequest::PREVIOUS_GAMES, UEXGameplayStatics::GetLocalSteamID(this)));
}

void UEXGameInstance::RequestGameInfo(uint64 GameId)
{
	MSSendData(FEXSerialize(EMSRequest::GAME_STATS, GameId));
}

void UEXGameInstance::ChangeIGN(const FString& IGN)
{
	MSSendData(FEXSerialize(EMSRequest::NAME_CHANGE, UEXGameplayStatics::GetLocalSteamID(this), IGN));
}

void UEXGameInstance::ClearIGN(const FString& SteamName)
{
	MSSendData(FEXSerialize(EMSRequest::CLEAR_NAME, UEXGameplayStatics::GetLocalSteamID(this), SteamName));
}

void UEXGameInstance::UpdateSquad(const FSquad& Squad)
{
	MSSendData(FEXSerialize(EMSRequest::SQUAD_CHANGE, UEXGameplayStatics::GetLocalSteamID(this), Squad));
}

void UEXGameInstance::ChangeSkin(EMerc Merc, uint8 EntityId, uint32 SkinId, EItemType SkinType)
{
	MSSendData(FEXSerialize(EMSRequest::CHANGE_SKIN, UEXGameplayStatics::GetLocalSteamID(this), Merc, EntityId, SkinId, SkinType));
}

void UEXGameInstance::RequestInventory()
{
	MSSendData(FEXSerialize(EMSRequest::INVENTORY, UEXGameplayStatics::GetLocalSteamID(this)));
}
void UEXGameInstance::GetShopItems()
{
	MSSendData(FEXSerialize(EMSRequest::SHOP_ITEMS, UEXGameplayStatics::GetLocalSteamID(this)));
}
void UEXGameInstance::BuyShopItem(uint32 IdItem, uint16 Count)
{
	MSSendData(FEXSerialize(EMSRequest::BUY_SHOP_ITEM, UEXGameplayStatics::GetLocalSteamID(this), IdItem, Count));
}

void UEXGameInstance::SubmitAssaultCourseResult(const FAssaultCourseResult& Result)
{
	MSSendData(FEXSerialize(EMSRequest::ASSAULT_COURSE_RESULT, UEXGameplayStatics::GetLocalSteamID(this), Result));
}

void UEXGameInstance::RequestAssaultCourseGlobalStats(const FString& Map)
{
	MSSendData(FEXSerialize(EMSRequest::ASSAULT_COURSE_STATS, Map));
}

void UEXGameInstance::RequestACInfo(const FString& Map)
{
	MSSendData(FEXSerialize(EMSRequest::ASSAULT_COURSE_INFO, Map));
}

void UEXGameInstance::SubmitGameResult(const FGameResultSW& GameResult)
{
	MSSendData(FEXSerialize(EMSRequest::GAME_RESULT_SW, GameResult));
}

void UEXGameInstance::SubmitGameResult(const FGameResultDM& GameResult)
{
	MSSendData(FEXSerialize(EMSRequest::GAME_RESULT_DM, GameResult));
}

// #MSRESPONSE
void UEXGameInstance::HandleDataReceived(FEXUnserialize& DataObj)
{
	const EMSResponse Request = DataObj.GetRequest();
	switch (Request)
	{
		case EMSResponse::INVALID:
		default:
		{
			UE_LOG(LogEXOnline, Log, TEXT("Received invalid request."));
			break;
		}
		case EMSResponse::CLIENT_INFO:
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_MSTimeout);
			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				FBasicInfo Info;
				DataObj.Get(Info);
				PC->LoadBasicInfo(MoveTemp(Info));
			}
			break;
		}
		case EMSResponse::OUTDATED_CLIENT:
		{
			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				PC->GetMainMenuWidget()->OutdatedClient();
			}
			break;
		}
		case EMSResponse::CURRENTLY_OFFLINE:
		{
			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				PC->GetMainMenuWidget()->CurrentlyOffline();
			}
			break;
		}
		case EMSResponse::PLAYER_INFO:
		{
			uint64 SteamId;
			DataObj.Get(SteamId);
			TArray<FSquadMerc> Squad;
			DataObj.Get(Squad);
			FString Name;
			DataObj.Get(Name);
			AEXGameModeBase* GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
			if (GM)
			{
				GM->LoadNewPLayer(SteamId, Squad, Name);
			}
			AEXGameModeLobby* GMLobby = GetWorld()->GetAuthGameMode<AEXGameModeLobby>();
			if (GMLobby)
			{
				GMLobby->LoadNewPLayer(SteamId, Squad, Name);
			}
			break;
		}
		case EMSResponse::SQUAD_CHANGE_SUCCESS:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Squad change success."));
			break;
		}
		case EMSResponse::NAME_CHANGE_SUCCESS:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Name change success."));
			break;
		}
		case EMSResponse::ASSAULT_COURSE_RESULT_SUCCESS:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Assault course result reported successfully."));
			break;
		}
		case EMSResponse::ASSAULT_COURSE_STATS:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Assault course stats received."));
			FAssaultCourseStats ACStats;
			DataObj.Get(ACStats);
			AEXAssaultCourseGame* GM = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
			GM->ReceiveGlobalStats(ACStats);
			break;
		}
		case EMSResponse::PROFILE_INFO:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Profile data received."));

			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				FPlayerStats PlayerStats;
				DataObj.Get(PlayerStats);
				PC->LoadProfileInfo(MoveTemp(PlayerStats));
			}
			break;
		}
		case EMSResponse::PREVIOUS_GAMES:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Previous game data received."));

			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				TArray<FGameInfo> OldGames;
				DataObj.Get(OldGames);
				PC->GetMainMenuWidget()->LoadOldGames(OldGames);
			}
			break;
		}
		case EMSResponse::GAME_STATS:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Single game data received."));

			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				uint64 GameId;
				DataObj.Get(GameId);
				FGameResultSW GameResult;
				DataObj.Get(GameResult);
				PC->GetMainMenuWidget()->LoadOldGame(GameId, GameResult);
			}
			break;
		}
		case EMSResponse::CHANGE_SKIN_SUCCESS:
		{
			UE_LOG(LogEXOnline, Warning, TEXT("Change skin success."));
			break;
		}
		case EMSResponse::GAME_RESULT_SUCCESS_SW:
		case EMSResponse::GAME_RESULT_SUCCESS_DM:
		{
			UE_LOG(LogEXOnline, Log, TEXT("Game result success."));
			break;
		}
		case EMSResponse::CLEAR_NAME_SUCCESS:
		{
			UE_LOG(LogEXOnline, Log, TEXT("Clear name success."));
			break;
		}
		case EMSResponse::INVENTORY:
		{
			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				TMap<uint32, uint32> OwnedItems;
				DataObj.Get(OwnedItems);
				PC->LoadPlayerInventory(OwnedItems);
			}
			break;
		}
		case EMSResponse::SHOP_ITEMS:
		{
			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				TArray<FShopItem> ShopItems;
				DataObj.Get(ShopItems);
				PC->GetMainMenuWidget()->LoadStoreItems(ShopItems);
			}
			break;
		}
		case EMSResponse::BUY_SHOP_ITEM_SUCCESS:
		{
			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				uint32 ItemId;
				DataObj.Get(ItemId);
				uint16 Count;
				DataObj.Get(Count);
				PC->ItemBought(ItemId, Count);
			}
			break;
		}
		case EMSResponse::BUY_SHOP_ITEM_FAIL:
		{
			AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
			if (PC)
			{
				PC->FailedToBuy();
			}
			break;
		}
		case EMSResponse::ASSAULT_COURSE_INFO:
		{
			AEXAssaultCourseGame* GM = GetWorld()->GetAuthGameMode<AEXAssaultCourseGame>();
			if (GM)
			{
				TMap<uint32, uint32> ACInfo;
				DataObj.Get(ACInfo);
				GM->LoadMedals(ACInfo);
			}
			break;
		}
	}
	if (DataObj.HasData())
	{
		HandleDataReceived(DataObj);
	}
}


bool UEXGameInstance::FindSessions(ULocalPlayer* PlayerOwner, bool bIsDedicatedServer)
{
	bool bResult = false;

	check(PlayerOwner);
	if (PlayerOwner)
	{
		AEXGameSession* const GameSession = GetGameSession();
		if (GameSession)
		{
			GameSession->OnFindSessionsComplete().RemoveAll(this);
			OnSearchSessionsCompleteDelegateHandle = GameSession->OnFindSessionsComplete().AddUObject(this, &UEXGameInstance::OnSearchSessionsComplete);

			GameSession->FindSessions(PlayerOwner->GetPreferredUniqueNetId().GetUniqueNetId(), NAME_GameSession, false, !bIsDedicatedServer);

			bResult = true;
		}
	}

	return bResult;
}

void UEXGameInstance::ReturnToMainMenu()
{
	Super::ReturnToMainMenu();

	AEXBaseController* PC = GetWorld()->GetFirstPlayerController<AEXBaseController>();
	if (PC)
	{
		if (PC->IsNetMode(NM_Standalone))
		{
			PC->LeaveOffileMap();
		}
		else
		{
			PC->EndSession();
		}
	}
}

void UEXGameInstance::StartServerSession_Implementation()
{
	/*UE_LOG(LogTemp, Warning, TEXT("StartServerSession_Implementation2."));
	TArray<FSessionPropertyKeyPair> Settings;
	FSessionPropertyKeyPair KP;
	KP.Key = FName("EX");
	KP.Data.SetValue(TEXT("EX"));
	Settings.Add(KP);
	FSessionPropertyKeyPair KP2;
	KP2.Key = FName("EX_Version");
	KP2.Data.SetValue(0.1);
	Settings.Add(KP2);
	UCreateSessionCallbackProxyAdvanced* Result = UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(this, Settings, nullptr, 16, 0, false, true, true, false, false, false, false, true, true);
	Result->OnSuccess.AddDynamic(this, &UEXGameInstance::SessionCreateSuccess);
	Result->OnFailure.AddDynamic(this, &UEXGameInstance::SessionCreateFailure);*/
}

void UEXGameInstance::MSConnect(bool bIgnoreEditorSetting)
{
	if(!bIgnoreEditorSetting) // Intended so that lobby still requests the master server connection, no point debugging without
	{
#if WITH_EDITOR && !USE_MASTER_SERVER_WITH_EDITOR
		return;
#endif
	}
	bWantsToConnect = true;
	float Time = GetWorld()->GetTimeSeconds();
	if (MSLastAttemptTime > 0.f && (Time - MSLastAttemptTime) < MSReconnectWaitTime) // Not first connection and trying to reconnect too soon
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_MSReconnect)) // Not waiting to reconnect yet
		{
			// Don't attempt to reconnect immediately
			UE_LOG(LogEXOnline, Warning, TEXT("Attempting to reconnect to the master server in %f."), MSReconnectWaitTime);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_MSReconnect, FTimerDelegate::CreateUObject(this, &UEXGameInstance::MSConnect, bIgnoreEditorSetting), MSReconnectWaitTime, false);
		}
	}
	else
	{
		// Connect to 
		UE_LOG(LogEXOnline, Warning, TEXT("Trying to connect to the master server (%s:%d)."), *MasterIP, MasterPort);
		TCPConn = GetWorld()->SpawnActor<ATcpSocketConnection>(TCPConnClass);
		TCPConn->Connect(MasterIP, MasterPort, DisconnectedDelegate, ConnectedDelegate, MessageReceivedDelegate, ConnectionId);
	}
	MSLastAttemptTime = Time;
}

void UEXGameInstance::MSDisconnect()
{
	bWantsToConnect = false;
	if (!bMSConnected)
	{
		return;
	}
	bMSConnected = false;
	TCPConn->Disconnect(ConnectionId);
}

void UEXGameInstance::ONMSConnected(int32 InConnectionId)
{
	UE_LOG(LogEXOnline, Warning, TEXT("Connected to the master server."));
	bMSConnected = true;

	if (Buffer.Num() > 0)
	{
		MSSendData(Buffer);
		Buffer.Empty();
	}
}

void UEXGameInstance::ONMSDisconnected(int32 InConnectionId)
{
	if (bWantsToConnect)
	{
		UE_LOG(LogEXOnline, Warning, TEXT("Lost connection to the master server."));
		bMSConnected = false;
		MSConnect();
	}
	else
	{
		UE_LOG(LogEXOnline, Warning, TEXT("Disconnected from the master server."));
	}
}

void UEXGameInstance::ONMSMessageReceived(int32 InConnectionId, TArray<uint8>& Message)
{
	UE_LOG(LogEXOnline, Warning, TEXT("Data received from the master server."));
	FEXUnserialize DataObj(Message);
	HandleDataReceived(DataObj);
}

void UEXGameInstance::MSSendData(const FEXSerialize& Data)
{
	MSSendData(Data.GetData());
}

void UEXGameInstance::MSSendData(const TArray<uint8>& Data)
{
	if (bMSConnected)
	{
		TCPConn->SendData(ConnectionId, Data);
	}
	else
	{
		Buffer.Append(Data);
	}
}

void UEXGameInstance::SessionCreateSuccess()
{
	UE_LOG(LogTemp, Error, TEXT("Server Success"));
}

void UEXGameInstance::SessionCreateFailure()
{
	UE_LOG(LogTemp, Error, TEXT("Server Failure"));
}

void UEXGameInstance::OnSearchSessionsComplete(bool bWasSuccessful)
{
	AEXGameSession* const Session = GetGameSession();
	if (Session)
	{
		Session->OnFindSessionsComplete().Remove(OnSearchSessionsCompleteDelegateHandle);
	}

	AEXGameSession* ShooterSession = GetGameSession();
	const TArray<FOnlineSessionSearchResult>& SearchResults = ShooterSession->GetSearchResults();

	for (const FOnlineSessionSearchResult& Result : SearchResults)
	{
		auto TServerName = Result.Session.OwningUserName;
		FString GameType;
		FString MapName;

		Result.Session.SessionSettings.Get(SETTING_GAMEMODE, GameType);
		Result.Session.SessionSettings.Get(SETTING_MAPNAME, MapName);
		UE_LOG(LogTemp, Error, TEXT("%s, %s"), *GameType, *MapName);
	}
}

void UEXGameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (MapName.Equals(FString("/Game/Maps/EntryMap")))
	{
		// Loading lobby
	}
	else
	{
		if (MapLoadWidget)
		{
			MapLoadWidget->Show(MapName);
			GetGameViewportClient()->AddViewportWidgetContent(MapLoadWidget->TakeWidget(), LOADING_MAP_WIDGET_Z_ORDER);
		}
	}
}

void UEXGameInstance::EndLoadingScreen(UWorld* LoadedWorld)
{
	if (MapLoadWidget)
	{
		GetGameViewportClient()->RemoveViewportWidgetContent(MapLoadWidget->TakeWidget());
	}
}

void UEXGameInstance::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	check(GetGameViewportClient());
	if (TravelWidget)
	{
		GetGameViewportClient()->AddViewportWidgetContent(TravelWidget->TakeWidget(), TRAVEL_WIDGET_Z_ORDER);
	}
}

void UEXGameInstance::HideTravelWidget()
{
	if (TravelWidget)
	{
		GetGameViewportClient()->RemoveViewportWidgetContent(TravelWidget->TakeWidget());
	}
	UE_LOG(LogTemp, Warning, TEXT("Hide travel widget"));
}

void UEXGameInstance::Timeout()
{
	MSDisconnect();
	AEXMenuController* PC = GetWorld()->GetFirstPlayerController<AEXMenuController>();
	if (PC)
	{
		PC->GetMainMenuWidget()->MSTimeout();
	}
}

