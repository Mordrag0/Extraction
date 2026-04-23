// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXTypes.h"
#include "EXGameStateBase.h"
#include "EXGameStateSW.generated.h"

class AEXPlayerController;
class AController;
struct FCanvasIcon;
class FUniqueNetId;
class AEXTeam;
class AEXPlayerState;


/**
 * 
 */
UCLASS()
class EX_API AEXGameStateSW : public AEXGameStateBase
{
	GENERATED_BODY()
	
public:
	AEXGameStateSW();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	virtual void StartRound() override;
	virtual void AdvanceRound() override;
	virtual void EndRound() override;

	FORCEINLINE uint8 GetRound() const { return Round; }
	FORCEINLINE uint8 GetNumRounds() const { return NumRounds; }



	void SetMatchStates(uint8 InAttSpawnWaveDuration, uint8 InDefSpawnWaveDuration, uint8 InNumRounds);

	bool IsOnLastRound() const;

	virtual void Init() override;
	virtual void AddObjectiveTime(int32 Stage) override;

	virtual void SendGameResult() const override;

	TArray<FRoundScore> GetRoundScores() const { return RoundScores; }

	FGameResultSW SerializeGameScore() const;

	virtual void Reset() override;

	virtual void ResetRound() override;

	void SetTeams(AEXTeam* InAttackers, AEXTeam* InDefenders, AEXTeam* InSpectators);

	FORCEINLINE AEXTeam* GetAttackers() const { return Attackers; }
	FORCEINLINE AEXTeam* GetDefenders() const { return Defenders; }
	FORCEINLINE AEXTeam* GetSpectators() const { return Spectators; }
	virtual TArray<AEXPlayerState*> GetSpectatorMembers() const override;
protected:

	virtual void CreateCurrentState(const FName& NewState, int32 NewStateDuration) override;


	FTeamResult GetTeamScore(AEXTeam* Team, TArray<AEXPlayerState*> InactivePlayers) const;

	UFUNCTION()
	void OnRep_CurrentMatchState();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentMatchState)
	FEXMatchStateSW CurrentMatchStateSW;

	// Objective stage
	UPROPERTY(Replicated)
	uint8 Round = 0;
	UPROPERTY(Replicated)
	uint8 NumRounds = 0;
	
	uint8 AttSpawnWaveDuration = 0;
	uint8 DefSpawnWaveDuration = 0;
	uint8 SpawnTimeBeginOffset = 0;


	class AEXGameModeSW* GM = nullptr;

	UPROPERTY(Replicated)
	TArray<FRoundScore> RoundScores;

	UPROPERTY(Replicated)
	AEXTeam* Attackers = nullptr;
	UPROPERTY(Replicated)
	AEXTeam* Defenders = nullptr;
	UPROPERTY(Replicated)
	AEXTeam* Spectators = nullptr;
};
