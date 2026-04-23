// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EXTypes.h"
#include "EXPlayerState.generated.h"

DECLARE_DELEGATE_OneParam(FNameChanged, const FString&);

class AEXTeam;
class AEXPlayerController;


/**
 * 
 */
UCLASS()
class EX_API AEXPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AEXPlayerState();

	virtual void ClientInitialize(class AController* C) override;

	FPlayerStats GetStats();

	void SetSteamID(uint64 Id);
	uint64 GetSteamID() const { return SteamId; }

	void SetController(AEXPlayerController* InEXPC);
	void SetTeam(AEXTeam* NewTeam);
	void SetGameStats(const FPlayerStats& InGameStats) { GameStats = InGameStats; }

	virtual void OnRep_PlayerName() override;

	FNameChanged OnNameChanged;

	FORCEINLINE AEXPlayerController* GetPC() const { return EXPC; }
protected:
	UFUNCTION()
	void OnRep_Team(AEXTeam* OldTeam);

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	AEXTeam* Team = nullptr;


	// Live stats
	UPROPERTY(Replicated)
	int32 Kills = 0;
	UPROPERTY(Replicated)
	int32 Assists = 0;
	UPROPERTY(Replicated)
	int32 Deaths = 0;
	UPROPERTY(Replicated)
	int32 PlayerScore = 0;

	// Post match and profile stats
	FPlayerStats GameStats;

	// Stored stats at the end of previous round
	int32 RoundKills = 0;
	int32 RoundAssists = 0;
	int32 RoundDeaths = 0;
	int32 RoundPlayerScore = 0;
	FPlayerStats RoundGameStats;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OverrideWith(APlayerState* PlayerState) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Client, Unreliable, Category = "Stats")
	void Client_Unreliable_AddScore(int32 AddedScore, EScoreType Type);
	UFUNCTION(Client, Reliable, Category = "Stats")
	void Client_Reliable_ResetScore(int32 OldPlayerScore);

public:
	UFUNCTION(BlueprintCallable, Category = "Team")
	FORCEINLINE AEXTeam* GetTeam() const { return Team; }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE int32 GetKills() const { return Kills; }
	void SetKills(int32 InKills) { Kills = InKills; }
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE int32 GetAssists() const { return Assists; }
	void SetAssists(int32 InAssists) { Assists = InAssists; }
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE int32 GetDeaths() const { return Deaths; }
	void SetDeaths(int32 InDeaths) { Deaths = InDeaths; }
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE int32 GetPlayerScore() const { return PlayerScore; }
	void SetPlayerScore(int32 InPlayerScore) { PlayerScore = InPlayerScore; }

	void SetRoundStats(int32 InKills, int32 InAssists, int32 InDeaths, int32 InScore, const FPlayerStats& InPlayerStats);

	void AddScore(int32 AddedScore, EScoreType ScoreType, bool bInstant = false);

	virtual void Reset() override;

	UFUNCTION(BlueprintCallable, Category = "Team")
	bool OnSameTeam(const AEXPlayerState* Other) const;

	UFUNCTION(BlueprintCallable, Category = "Team")
	bool IsTeam(ETeam InTeam) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void MercStarted(EMerc Merc);
	void MercStopped();
	void WeaponEquipped(EWeapon Weapon);
	void WeaponUnequipped();
	void AbilityEquipped(EAbility Ability);
	void AbilityUnequipped();

	void ShotMissed(EMerc Merc, EWeapon Weapon);
	void ShotHit(EMerc Merc, EWeapon Weapon, bool bHeadshot, bool bKill, float Damage);
	void AbilityUsed(EMerc Merc, EAbility Ability);
	void AbilityHit(EMerc Merc, EAbility Ability, bool bHit, bool bHeadshot, bool bKill, float Damage);
	void AddRevive(EMerc Merc, float HealthGiven);
	void AddKill(EMerc Merc);
	void AddDeath(EMerc Merc);
	void AddAssist(EMerc Merc);
	void DamageTaken(EMerc Merc, float Damage);

	// Store stats from before the round
	void StartRound();
	// Revert to the stats from before the round
	void ResetRound();

	EMerc GetCurrentMerc() const { return CurrentMerc; }
	void SetSquad(const TArray<FSquadMerc>& InSquad);
	TArray<FSquadMerc> GetSquad() const { return Squad; }
	TArray<EAugment> GetAugments(EMerc MercType) const;

protected:

	TMap<EScoreType, int32> TempScores;

	UPROPERTY(Replicated)
	TArray<FSquadMerc> Squad;

	EMerc CurrentMerc = EMerc::Default;
	float MercSelectTime = 0.f;
	EWeapon CurrentWeapon = EWeapon::None;
	float WeaponSelectTime = 0.f;
	EAbility CurrentAbility = EAbility::None;
	float AbilitySelectTime = 0.f;

	UPROPERTY()
	AEXPlayerController* EXPC = nullptr;

	UPROPERTY(Replicated)
	uint64 SteamId = 0;

	class AEXGameStateBase* GS = nullptr;
};
