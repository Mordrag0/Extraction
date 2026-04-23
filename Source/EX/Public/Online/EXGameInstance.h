// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TcpSocketPlugin/Public/TcpSocketConnection.h"
#include "EXTypes.h"
#include "Online/EXSerialization.h"
#include "EXGameInstance.generated.h"

class AEXGameSession;
class AEXCharacter;
class AEXPlayerState;
class UEXLoadingMap;

namespace EXGameInstanceState
{
	extern const FName None;
	extern const FName PendingInvite;
	extern const FName WelcomeScreen;
	extern const FName MainMenu;
	extern const FName MessageMenu;
	extern const FName Playing;
}

/**
 * 
 */
UCLASS()
class EX_API UEXGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void Init() override;
	void Shutdown() override;

	AEXGameSession* GetGameSession() const;

// #MasterServer
	void RequestClientInfo();
	void RequestPlayerInfo(AEXPlayerState* PS);
	void RequestProfileInfo();
	void RequestOldGames();
	void RequestGameInfo(uint64 GameId);

	void ChangeIGN(const FString& IGN);
	void ClearIGN(const FString& SteamName);
	void UpdateSquad(const FSquad& Squad);

	void ChangeSkin(EMerc Merc, uint8 EntityId, uint32 SkinId, EItemType SkinType);

	void RequestInventory();
	void GetShopItems();
	void BuyShopItem(uint32 IdItem, uint16 Count);

	void SubmitAssaultCourseResult(const FAssaultCourseResult& Result);
	void RequestAssaultCourseGlobalStats(const FString& Map);
	void RequestACInfo(const FString& Map);

	void SubmitGameResult(const FGameResultSW& GameResult);
	void SubmitGameResult(const FGameResultDM& GameResult);

	// MasterServer

	//protected:

	/** Initiates the session searching */
	UFUNCTION(BlueprintCallable, Category = "Game Instance")
	bool FindSessions(ULocalPlayer* PlayerOwner, bool bIsDedicatedServer);

	UFUNCTION(BlueprintNativeEvent, Category = "Session")
	void StartServerSession();


	virtual void ReturnToMainMenu() override;

#pragma region Master
public:
	void MSConnect(bool bIgnoreEditorSetting = false);
	void MSDisconnect();
protected:
	void MSSendData(const FEXSerialize& Data);
	void MSSendData(const TArray<uint8>& Data);

	UFUNCTION()
	void ONMSConnected(int32 InConnectionId);
	UFUNCTION()
	void ONMSDisconnected(int32 InConnectionId);
	UFUNCTION()
	void ONMSMessageReceived(int32 InConnectionId, TArray<uint8>& Message);

	void HandleDataReceived(FEXUnserialize& DataObj);

	UPROPERTY(EditDefaultsOnly, Category = "Master")
	TSubclassOf<ATcpSocketConnection> TCPConnClass;
	UPROPERTY(EditDefaultsOnly, Category = "Master")
	float MSTimeoutTime = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Master")
	float MSReconnectWaitTime = 10.f;
	float MSLastAttemptTime = -1.f;

	UPROPERTY(Config)
	FString MasterIP;
	UPROPERTY(Config)
	int32 MasterPort;

private:
	TArray<uint8> Buffer;
	bool bMSConnected = false;
	bool bWantsToConnect = false;
	ATcpSocketConnection* TCPConn = nullptr;
	int32 ConnectionId = -1;
	FTcpSocketDisconnectDelegate DisconnectedDelegate;
	FTcpSocketConnectDelegate ConnectedDelegate;
	FTcpSocketReceivedMessageDelegate MessageReceivedDelegate;
	FTimerHandle TimerHandle_MSReconnect;
#pragma endregion Master

protected:
	// #ADD_MERC
	UPROPERTY(EditDefaultsOnly, Category = "Squad")
	TMap<EMerc, TSoftClassPtr<AEXCharacter>> MercClassesToLoad; 

public:
	TMap<EMerc, TSoftClassPtr<AEXCharacter>> GetMercClasses() const { return MercClassesToLoad; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Travel")
	TSubclassOf<UUserWidget> TravelWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* TravelWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Travel")
	TSubclassOf<UEXLoadingMap> MapLoadWidgetClass = nullptr;
	UPROPERTY()
	UEXLoadingMap* MapLoadWidget = nullptr;

private:

	UFUNCTION()
	void SessionCreateSuccess();
	UFUNCTION()
	void SessionCreateFailure();

	/** Handle to various registered delegates */
	FDelegateHandle OnSearchSessionsCompleteDelegateHandle;

	/** Callback which is intended to be called upon finding sessions */
	void OnSearchSessionsComplete(bool bWasSuccessful);

	void BeginLoadingScreen(const FString& MapName);
	void EndLoadingScreen(UWorld* LoadedWorld);

	UFUNCTION()
	void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel);
public:
	void HideTravelWidget();

private:
	UFUNCTION()
	void Timeout();

	FTimerHandle TimerHandle_MSTimeout;

};
