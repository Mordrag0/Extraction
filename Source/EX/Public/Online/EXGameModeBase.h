// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Engine/DataTable.h"
#include "EXGameModeBase.generated.h"

class AEXBaseController;
class AEXPlayerController;
class AEXLevelRules;
class AEXTeam;
class AEXGameStateBase;
struct FEXMatchState;
class UEXScoreboardBase;
class AEXCharacter;
struct FSquadMerc;
class AEXPlayerStart;
class AEXInteract;
class AEXFlag;
class AEXGameSession;
class UDataTable;
class UEXWeaponData;

USTRUCT(BlueprintType)
struct FCharacterTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AEXCharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = 100.f;

};

USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UEXWeaponData* WeaponData;


};

/**
 * 
 */
UCLASS()
class EX_API AEXGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:
	AEXGameModeBase();

	AEXGameSession* GetGameSession() const { return EXGameSession; }

	UFUNCTION(BlueprintNativeEvent, Category = "Chat")
	bool AllowTextMessage(const FString& Msg, bool bIsTeamMessage, AEXBaseController* Sender) const;

	bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;

	bool ClearPause() override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;


	virtual void StartToLeaveMap() override;

	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	void KillAllPlayers();

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual void EndMatch() override;

	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;

	UFUNCTION(BlueprintCallable, Category = "XP")
	FORCEINLINE float GetGibbingXPModifier() const { return GibbingXPModifier; }
	UFUNCTION(BlueprintCallable, Category = "XP")
	FORCEINLINE int32 GetScorePerMagGiven() const { return XPPerMagGiven; }

	// Can the player deal damage according to gamemode rules (e.g. friendly-fire disabled)
	virtual bool CanDealDamage(class AEXPlayerState* DamageCauser, class AEXPlayerState* DamagedPlayer) const { return true; }

	bool RequestAdmin(AEXPlayerController* Player, const FString& Password);
	FORCEINLINE bool IsAdmin(AEXPlayerController* Player) const { return Admins.Contains(Player); }

	FORCEINLINE bool IsRanked() const { return bRanked; }

	virtual void ResetLevel();

	virtual void InitTeams() {}

	void ForceStartMatch();

	virtual void StartMatch() override;


	virtual void InitGameState() override;

	virtual void AdminSetTime(uint32 Time);

	virtual void ChangeState(FEXMatchState* InMatchState);

	TArray<AEXPlayerController*> GetConnectedPlayers() const { return ConnectedPlayers; }

	virtual void RestartRound() {}
	virtual void SwapTeams() {}

	//~ Begin AGameModeBase Interface
protected:
	virtual void OnMatchStateChanged();

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;


	UPROPERTY()
	TArray<UObject*> LoadedAssets;

	AEXPlayerController* GetPlayer(uint64 SteamId) const;
	void LoadServerAssets(const TArray<TSoftClassPtr<AEXCharacter>>& Mercs);
public:
	void LoadNewPLayer(uint64 SteamId, const TArray<FSquadMerc>& Squad, const FString& Name);

	void PostLogin(APlayerController* NewPlayer);
	virtual void Logout(AController* Exiting) override;
	//~ End AGameModeBase Interface

	virtual void AddToSpectators(AEXPlayerController* PC) {}
	FORCEINLINE bool IsTeamSwitchAllowed() const { return bAllowTeamSwitch; }
	virtual void SwitchTeam(AEXPlayerController* PC) {}

	uint8 GetMaxPlayersPerTeam() const { return MaxPlayersPerTeam; }

protected:
	virtual void OnMatchStateSet() override;

	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchPreRound();
	virtual void HandleOvertime();
	virtual void HandleMatchPostRound();
	virtual void HandleMatchHasEnded() override;

	virtual void StartPreRound();
	virtual void StartRound();
	virtual void StartPostRound();

	virtual bool ReadyToStartMatch_Implementation() override;

	UPROPERTY(EditDefaultsOnly, Category = "Teams")
	TSubclassOf<AEXTeam> TeamClass;
	UPROPERTY(EditDefaultsOnly, Category = "Teams")
	bool bAllowTeamSwitch = false;

	bool bTeamGame = false;

	virtual bool ShouldResetActor(AActor* ActorToReset);

	void PauseStatusChanged(bool bNewPaused);

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);

	TSubclassOf<AGameSession> GetGameSessionClass() const;

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	float HeadShotMultiplier = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	float LimbMultiplier = .5f;
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	float LowerLimbMultiplier = .3f;

	UPROPERTY(EditDefaultsOnly, Category = "XP")
	float GibbingXPModifier = .5f;
	UPROPERTY(EditDefaultsOnly, Category = "XP")
	int32 XPPerMagGiven = 25;

	UPROPERTY(Config)
	uint8 MaxPlayersPerTeam;

	UPROPERTY()
	AEXLevelRules* LevelRules = nullptr;

	virtual void BeginPlay() override;

	void WarmUpTimerDone();
	void StartMatchIfConditionsMet();

public:
	bool GetReduceCooldownWhenDead() const { return bReduceCooldownWhenDead; }
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bReduceCooldownWhenDead = true;
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bRanked = false;

	bool bFullReset = false;
public:
	/// Admin config overrides
	void SetNumPlayersRequiredToStart(uint8 InNumPlayersRequiredToStart);
	///~ Admin config overrides
protected:
	UPROPERTY(Config)
	uint8 NumPlayersRequiredToStart;

	int32 GetStateDuration(const FName& InMatchState) const;

	UPROPERTY()
	TArray<AEXPlayerController*> ConnectedPlayers;

public:
	virtual void Second();
	// Called to signal end of PreRound, Round, Overtime or PostRound
	virtual void EndRoundState();

	int32 RoundSeconds = 0;

	virtual void CharacterDied(AEXPlayerController* PC) {}

	virtual void FlagCaptured(AEXFlag* Flag, bool bRecaptured);

	virtual void AdvanceStage(int32 Stage);

protected:

	FTimerHandle TimerHandle_Second;

	FName CurrentState;

private:
	UPROPERTY(Config)
	TArray<FString> ServerPasswords;

	TArray<AEXPlayerController*> Players;
	UPROPERTY()
	TSet<AEXPlayerController*> Admins;

	bool bWarmupTimerDone = false;
	FTimerHandle TimerHandle_WarmupTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	int32 WarmupMaxDuration = 30.f;

	UPROPERTY(Config)
	int32 PreRoundTime;
	UPROPERTY(Config)
	int32 PostRoundTime;
	UPROPERTY(Config)
	int32 RoundTime;

	AEXGameStateBase* GSB = nullptr;

#if WITH_EDITOR
	int32 PlayerIdx = 1;
#endif

protected:
	/// #Beacons
	UPROPERTY()
	class AEXOnlineBeaconHostObject* OBHostObject = nullptr;
	UPROPERTY()
	class AEXOnlineBeaconHost* OBHost = nullptr; // #EXTODO2
	///

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void AddBots(int32 Count);
	UFUNCTION(BlueprintCallable, Category = "AI")
	void RemoveAllBots();
protected:
	// #AI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TSubclassOf<APawn> BotClass = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BotBehaviorTree = nullptr;

	UPROPERTY()
	TArray<AEXCharacter*> Bots;

	UPROPERTY()
	TArray<AEXPlayerStart*> PlayerStarts;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* CharacterTable;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* InventoryTable;

	UPROPERTY()
	AEXGameSession* EXGameSession = nullptr;
};
