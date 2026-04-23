// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXGameModeBase.h"
#include "EXTypes.h"
#include "EXGameModeSW.generated.h"

class AEXPlayerState;
class AEXLevelRules;
class AEXPlayerController;
class AEXTeam;
class AEXSpawnArea;
class AEXFlag;

UCLASS(minimalapi)
class AEXGameModeSW : public AEXGameModeBase
{
	GENERATED_BODY()

public:
	AEXGameModeSW();

	void HalfTime();

	void RestartRound() override;


	virtual void ResetLevel() override;

	void SwapTeams() override;

	virtual void FlagCaptured(AEXFlag* Flag, bool bRecaptured) override;

	void AdvanceStage(int32 Stage) override;

protected:
	virtual void SetSpawnsForStage(int32 Stage);

	virtual void AutoAssignTeam(AEXPlayerController* Player, bool bForceSpec);

	void BeginPlay() override;

public:

	//~ Begin AGameModeBase Interface
	//virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	//virtual void StartPlay() override;
	//virtual bool HasMatchStarted() const override;
	//virtual void PostLogin(APlayerController* NewPlayer) override;
	//virtual void Logout(AController* Exiting) override;
	virtual int32 GetNumPlayers() override;
	virtual int32 GetNumSpectators() override;
	//virtual bool IsHandlingReplays() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	//virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	//virtual void PostSeamlessTravel() override;
	//virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	//virtual void InitSeamlessTravelPlayer(AController* NewController) override;
	//virtual bool CanServerTravel(const FString& URL, bool bAbsolute) override;
	//virtual void StartToLeaveMap() override;
	//virtual bool SpawnPlayerFromSimulate(const FVector& NewLocation, const FRotator& NewRotation) override;
	//~ End AGameModeBase Interface
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bFriendlyFire = true;

public:
	bool IsInitialized() const;

	virtual void InitTeams() override;

	void ResetSpawns();

	virtual bool MustSpectate_Implementation(APlayerController* NewPlayerController) const override;

	virtual void InitGameState() override;


	// Can the player deal damage according to gamemode rules (e.g. friendly-fire disabled)
	virtual bool CanDealDamage(class AEXPlayerState* DamageCauser, class AEXPlayerState* DamagedPlayer) const override;


	UFUNCTION(BlueprintCallable, Category = "Rules")
	FORCEINLINE uint8 GetSpawnWaveTime(ETeam Type) const { return (Type == ETeam::Attack) ? AttackSpawnWaveTime : DefenseSpawnWaveTime; }

	UFUNCTION()
	virtual void SpawnWave(ETeam TeamType);


	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	AEXTeam* GetAttackersOnStart() const;
	AEXTeam* GetDefendersOnStart() const;
	FORCEINLINE AEXTeam* GetAttackers() const {	return Attackers; }
	FORCEINLINE AEXTeam* GetDefenders() const {	return Defenders; }
	FORCEINLINE AEXTeam* GetSpectators() const { return Spectators; }
	virtual void AddToSpectators(AEXPlayerController* PC);

	virtual void SwitchTeam(AEXPlayerController* PC) override;

protected:

	UPROPERTY()
	AEXTeam* Attackers = nullptr;
	UPROPERTY()
	AEXTeam* Defenders = nullptr;
	UPROPERTY()
	AEXTeam* Spectators = nullptr;


	UPROPERTY(Config)
	uint8 AttackSpawnWaveTime = 20;
	UPROPERTY(Config)
	uint8 DefenseSpawnWaveTime = 25;

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	int32 NumRounds = 2;

	virtual void ChangeState(FEXMatchState* InMatchState) override;
	virtual void OnMatchStateChanged() override;
	virtual void UpdateClientTimers(AEXPlayerController* PC);
public:
	virtual void Second() override;
	virtual void EndRoundState() override;
protected:
	int32 AttSpawnSeconds = 0;
	int32 DefSpawnSeconds = 0;
	int32 AttSpawnWaveDuration = 0;
	int32 DefSpawnWaveDuration = 0;

private:
	UPROPERTY()
	class AEXGameStateSW* GSB = nullptr;
};



