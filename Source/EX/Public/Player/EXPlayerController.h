// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXBaseController.h"
#include "EXCharacter.h"
#include "EXTypes.h"
#include "Online/EXPlayerState.h"
#include "EXPlayerController.generated.h"

class UEXHUDWidget;
class AEXTeam;
class UEXWeapon;
class UEXInventory;
class UUserWidget;
class AEXCharacter;
class UEXRoundEndScreen;
class UEXDeathScreen;
class AEXLevelRules;
class UTexture;
class UEXInGameMenu;
class UEXQuickChatWindow;
class UEXGameStatus;
class UEXPlayerContextMenu;
class AEXPlayerState;
class UEXScoreboardBase;
class AEXGameStateBase;
class AEXPlayerCameraManager;
class UEXAdminPanel;
class UEXTextSelectPanel;
class UEXPersistentUser;
class UEXAbilityTracker;

enum class EScoreType : uint8;

struct FTextPanelCategory
{
public:
	using TextPanelFunction = void (AEXPlayerController::*)(FTextPanelCategory& Line);
	FTextPanelCategory() {}
	FTextPanelCategory(TextPanelFunction InFunc, UEXTextSelectPanel* InPanel, const FText& InName)
		: Func(InFunc), Panel(InPanel), Name(InName) { }
	FTextPanelCategory(TextPanelFunction InFunc, UEXTextSelectPanel* InPanel, const FText& InName, const TArray<FTextPanelCategory>& InCategories)
		: Func(InFunc), Panel(InPanel), Name(InName), Categories(InCategories) { }
	FTextPanelCategory(TextPanelFunction InFunc, UEXTextSelectPanel* InPanel, const FText& InName, int32 InOption)
		: Func(InFunc), Panel(InPanel), Name(InName), Option(InOption) { }

	TextPanelFunction Func;
	UEXTextSelectPanel* Panel = nullptr;
	FText Name;
	TArray<FTextPanelCategory> Categories;
	int32 Option;

	TArray<FText> GetTextOptions() const
	{
		TArray<FText> CategoryNames;
		for (const FTextPanelCategory& Category : Categories)
		{
			CategoryNames.Add(Category.Name);
		}
		return CategoryNames;
	}
};

/**
 * 
 */
UCLASS()
class EX_API AEXPlayerController : public AEXBaseController
{
	GENERATED_BODY()
	
public:
	AEXPlayerController();

	FORCEINLINE UEXHUDWidget* GetHUDWidget() const override { return HUD; }
	AEXCharacter* GetEXCharacter() const { return EXCharacter; } 
	TArray<TSoftClassPtr<AEXCharacter>> GetSelectedMercs() const { return SelectedMercs; }
	void SetSelectedMercs(const TArray<TSoftClassPtr<AEXCharacter>>& InSelectedMercs);

	UFUNCTION(Server, Reliable)
	void Server_Reliable_OnClientInit();

	UFUNCTION(Client, Reliable)
	void Client_Reliable_Init(const TArray<TSoftClassPtr<AEXCharacter>>& ClassesToLoadOnNewPlayer, const TArray<FSquadMerc>& InSquad);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_LoadAssets(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs);

	void DisplayMessage(FText Message);

	void OnDeath();

	virtual void Reset() override;

	UPROPERTY()
	TArray<UObject*> LoadedAssets; // #EXTODO do we need this?

	void LoadAssets(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs);

	void UnloadAssets(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs);

	virtual bool CanRestartPlayer() override;

	virtual void SetPawn(APawn* InPawn) override;

	virtual void OnRep_Pawn() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitPlayerState() override;

	void SpawnPlayerCameraManager() override;

	// #VOTE
	void UpdateVote(int32 Yeses, int32 Nos);
	void StartVote(const FVoteInfo& Vote);

	void EndVote();

	UFUNCTION(Server, Unreliable)
	void Server_Unreliable_Vote(bool bVote);

	void VoteYes();
	void VoteNo();
	void OnVote(bool bVote);
protected:
	bool bCanVote = false;

public:
	UEXAbilityTracker* GetAbilityTracker() const { return AbilityTracker; }
protected:
	UPROPERTY()
	UEXAbilityTracker* AbilityTracker;

	UPROPERTY(EditDefaultsOnly, Category = "ModeSetting")
	TSubclassOf<UEXScoreboardBase> ScoreboardClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "ModeSetting")
	TSubclassOf<UEXScoreboardBase> EndScoreboardClass = nullptr;

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	bool CanRespawn() const;

	virtual void Respawn();

	UFUNCTION(Server, Reliable)
	virtual	void Server_Reliable_Respawn();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual	void Server_Reliable_ChoosePawnClass(UClass* PawnClass);

	// Set to false after respawning or being revived and set to true on spawn wave
	UPROPERTY(BlueprintReadOnly)
	bool bCanRespawn = true; // True by default so that you don't have to wait for the next spawn wave when you join mid match

	void SetCanRespawn(bool bInCanRespawn);
	UFUNCTION(Client, Reliable)
	void Client_Reliable_SetCanRespawn(bool bInCanRespawn);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UEXHUDWidget> HUDClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UEXHUDWidget> OfflineHUDClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UEXInGameMenu> InGameMenuClass = nullptr;

	UPROPERTY()
	UEXHUDWidget* HUD = nullptr;
	UPROPERTY()
	UEXScoreboardBase* Scoreboard = nullptr;
	UPROPERTY()
	UEXScoreboardBase* RoundEndScreen = nullptr;
	UPROPERTY()
	UEXInGameMenu* InGameMenu = nullptr;
	UPROPERTY()
	UEXTextSelectPanel* QuickChatWindow = nullptr;
	UPROPERTY()
	UEXTextSelectPanel* AdminPanel = nullptr;

	UFUNCTION()
	void ShowScoreboard();
	UFUNCTION()
	void HideScoreboard();

	template<bool bSpeaking> 
	UFUNCTION()
	void PushToTalk();
	UFUNCTION()
	void Cancel();

	/// Text panels
	void QuickChat();
	void OpenAdminPanel();

	void OpenTextPanel(UEXTextSelectPanel* Panel, const FTextPanelCategory& Options);

#pragma region Panel
	// #PANEL
	void ExpandPanel(FTextPanelCategory& Cat);
	void SayQuickChat(FTextPanelCategory& Cat);
	void SayQuickChatTeam(FTextPanelCategory& Cat);
	void CreateKickOptions(FTextPanelCategory& Cat);
	void CreateMapOptions(FTextPanelCategory& Cat);
	void StartVoteKick(FTextPanelCategory& Cat);
	void AdminChangeMap(FTextPanelCategory& Cat);
	void AdminStartMatch(FTextPanelCategory& Cat);
#pragma endregion Panel

	UFUNCTION()
	void Click();

	template<int32 Index>
	void QuickSelect();

	void QuickSelectTextPanel(UEXTextSelectPanel* Panel, int32 Idx);

	void CreateHUD();
	void CreateQuickChat();
	void CreateAdminPanel(bool bInAdmin);

	template<bool bShow>
	void ShowObjectives() { ShowObjectives(bShow); }

	UFUNCTION()
	void ShowObjectives(bool bShow);


	UFUNCTION(Client, Reliable)
	void Client_Reliable_PauseStatusChanged(bool bNewPaused);

	virtual void ClientWasKicked_Implementation(const FText& KickReason) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

public:
	void PauseStatusChanged(bool bNewPaused);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_SetInteractProgressVisibility(bool bVisible);


	UFUNCTION()
	virtual void CharacterRevived();
	UFUNCTION()
	virtual void CharacterDied();

	// Called every spawn wave
	UFUNCTION()
	void AllowRespawn();


	virtual void OnRep_PlayerState() override;

	void OnTeamChanged(AEXTeam* Team);

	void UpdateAmmo(const UEXWeapon* Weapon);

	void UpdateInventory(const UEXInventory* Inventory);

	void SetScopeVisibility(bool bVisible);
	void SetCrosshairVisibility(bool bVisible);

	void SetSpread(float Val);

	void SetSpotted(bool bSpotted);

	FORCEINLINE UClass* GetSelectedClass() const { return SelectedPlayerClass; }

	void SetInteractionIconVisibility(bool bVisible);

	void OnScoreAdded(int32 AddedScore, EScoreType Type);

	void AddToKillFeed(const FString& DeadPlayer, UTexture2D* KillIcon, const FString& Killer);

	void RefreshGameStatusWidget(AEXTeam* Attackers, AEXTeam* Defenders);

	void HitOnClient(bool bHeadshot);



	FORCEINLINE AEXLevelRules* GetLevelRules() const { return LevelRules; }

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	virtual void InitInputSystem() override;

	virtual void PostInitProperties() override;

	virtual void ReceivedPlayer() override;

	void UpdateTimers(int32 InRoundSeconds, int32 InSpawnSeconds);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_OnMatchStateChanged(const FName& NewMatchState);

	/** Starts the online game using the session name in the PlayerState */
	UFUNCTION(Client, Reliable)
	void Client_Reliable_StartOnlineGame();

	/** Ends the online game using the session name in the PlayerState */
	UFUNCTION(Client, Reliable)
	void Client_Reliable_EndOnlineGame();

	/** Sets spectator location and rotation */
	UFUNCTION(Client, Reliable)
	void Client_Reliable_SetSpectatorCamera(FVector CameraLocation, FRotator CameraRotation);

	FORCEINLINE bool IsAdmin() const { return bAdmin; }
	UFUNCTION(Client, Reliable)
	void Client_Reliable_EnableAdmin();
private:
	bool bAdmin = false;
	
protected:
	void SetDefaultFOV(float FOV);

	void PlayHeadshotSound();

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundCue* HeadshotSound = nullptr;

	FTimerHandle TimerHandle_MatchState;
	int32 MatchStateRemainingSeconds = 0;
	int32 SpawnWaveTime = 0;

	UFUNCTION()
	virtual void MercSelect(int32 Index);

	template<int32 Index>
	void MercSelect()
	{
		MercSelect(Index);
	}

	virtual void OpenChat(bool bTeam) override;

	UPROPERTY()
	class AEXLevelRules* LevelRules = nullptr;
	UPROPERTY()
	class AEXGameModeBase* GMB = nullptr;
	UPROPERTY()
	class AEXGameStateBase* GSB = nullptr;
	UPROPERTY()
	class AEXGameStateSW* GSSW = nullptr;

	TSubclassOf<AEXCharacter> SelectedPlayerClass = nullptr;

	TArray<TSoftClassPtr<AEXCharacter>> SelectedMercs;

	bool bPickUpIconVisible = false;

	bool bHitMarkersEnabled = true;
	bool bHideCrosshairWhenADS = false;

	FTextPanelCategory QuickChatOptions;
	FTextPanelCategory AdminCommands;

public:
	void Report(const AEXPlayerState* PS);
	UFUNCTION(Server, Reliable)
	void Server_Reliable_Report(const AEXPlayerState* PS, EReportReason Reason, const FString& Message);

	void SetClientDataLoaded() { bClientDataLoaded = true; }

private:
	bool bClientDataLoaded = false;
	bool bAssetsLoaded = false;

#pragma region ConsoleCommands
public:
	UFUNCTION(Exec)
	void Disconnect();
protected:
	UFUNCTION(Exec)
	void AssaultCourseReset();
	UFUNCTION(Exec)
	void SetFOV(float FOV);
	UFUNCTION(Exec)
	void SwitchTeam();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_SwitchTeam();
	UFUNCTION(Exec)
	void Spectate();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_Spectate();
	UFUNCTION(BlueprintImplementableEvent, Category = "Session")
	void OnDisconnect();
	UFUNCTION(Exec)
	void SetSensitivity(float Value);
	UFUNCTION(Exec)
	void SetAltInputs();
	// Master volume between 0 and 1
	UFUNCTION(Exec)
	void SetMasterVolume(float Value);
#pragma region Admin
	// Admin commands
	UFUNCTION(Exec)
	void AdminLogin(const FString& Password);
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminLogin(const FString& Password);
	UFUNCTION(Exec)
	void AdminSetRoundTime(const uint32 Time);
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminSetRoundTime(const uint32 Time);
	UFUNCTION(Exec)
	void AdminEndState();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminEndState();
	UFUNCTION(Exec)
	void AdminSwapTeams();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminSwapTeams();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminStartMatch();
	UFUNCTION(Exec)
	void AdminRestartLevel();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminRestartLevel();
	UFUNCTION(Exec)
	void AdminRestartRound();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminRestartRound();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminChangeLevel(const FName& MapName);
	UFUNCTION(Exec)
	void AdminSetNumPlayersRequiredToStart(uint8 NumPlayers);
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminSetNumPlayersRequiredToStart(uint8 NumPlayers);
	UFUNCTION(Exec)
	void AdminExecuteCommand();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AdminExecuteCommand();
	UFUNCTION()
	void VoteKickPlayer(AEXPlayerState* Target);
	UFUNCTION(Server, Reliable)
	void Server_Reliable_VoteKickPlayer(AEXPlayerState* Target);
	UFUNCTION(Exec)
	void HideHUD();
	UFUNCTION(Exec)
	void ShowHUD();
#pragma endregion Admin
#pragma region TestCommands
protected:
	UFUNCTION(Exec)
	void Test();
	UFUNCTION(Exec)
	void TestServer();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_Test();
	UFUNCTION(Exec)
	void TestShowPlayerStats();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_RequestPlayerStats();
	UFUNCTION(Client, Reliable)
	void Client_Reliable_DisplayPlayerStats(const TArray<FPlayerStats>& Stats);
	UFUNCTION(Exec)
	void Give();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_Give();
	UFUNCTION(Exec)
	void SendData();
	UFUNCTION(Server, Reliable)
	void Server_Reliable_SendData();

	UFUNCTION(Exec)
	void AddBots(int32 Count);
	UFUNCTION(Server, Reliable)
	void Server_Reliable_AddBots(int32 Count);
	UFUNCTION(Exec)
	void TestTeamHUD();

#pragma endregion TestCommands
#pragma endregion ConsoleCommands

protected:
	void LogAdminCommand(FString LogText);

	UPROPERTY(EditDefaultsOnly, Category = "ModeSetting")
	bool bTeamGame = false;

private:

	FTimerHandle TimerHandle_RequestTime;

	// Has time been synced with the server?
	bool bTimeSynced = false;
	// Has the level been initialized?
	bool bInitialized = false;


	/** Handle for efficient management of ClientStartOnlineGame timer */
	FTimerHandle TimerHandle_ClientStartOnlineGame;

public:
	void OverridePlayerState(AEXPlayerState* InPS);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_SetTimer(float WorldTime, int32 InRoundSeconds, int32 InSpawnSeconds, int32 InSpawnWave);

	FORCEINLINE AEXPlayerCameraManager* GetPCM() const { return PCM; }
private:
	int32 RoundSeconds = 0;
	int32 SpawnSeconds = 0;
	int32 SpawnWave = 0;
	UFUNCTION()
	void Second();
	FTimerHandle TimerHandle_Second;

	UPROPERTY()
	AEXCharacter* EXCharacter = nullptr;
	UPROPERTY()
	UEXPersistentUser* PersistentUser = nullptr;
	UPROPERTY()
	AEXPlayerCameraManager* PCM = nullptr;
};

