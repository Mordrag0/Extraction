// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXTypes.h"
#include "GameFramework/GameState.h"
#include "Templates/SharedPointer.h"
#include "EXGameStateBase.generated.h"

class AEXPlayerController;
class AEXLevelRules;
class AEXGameModeBase;
class AEXPlayerState;

enum class EGameMode
{
	Invalid,
	StopWatch,
	DeathMatch,
};

/**
 * 
 */
UCLASS()
class EX_API AEXGameStateBase : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void Init();

	virtual void AddObjectiveTime(int32 Stage);

	virtual bool IsMatchInProgress() const override;

	//~ Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

	void SetMatchState(const FName& NewState, int32 NewStateDuration);

	virtual void StartRound();

	virtual void EndRound();

	virtual void AdvanceRound();

	virtual void ResetRound() {}

	// If this returns false, then we shouldn't allow the client to spawn in
	bool GameReady() const;

	virtual void SendGameResult() const {}

	bool IsRoundInProgress() const { return CurrentMatchState->State == MatchState::InProgress; }

	virtual TArray<AEXPlayerState*> GetSpectatorMembers() const {return TArray<AEXPlayerState*>(); }


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddPlayerState(APlayerState* PlayerState) override;

	void RemovePlayerState(APlayerState* PlayerState) override;

	FORCEINLINE TArray<AEXPlayerState*> GetEXPlayerArray() const { return EXPlayerArray; }
	AEXPlayerState* GetPlayer(int32 PlayerId) const;
protected:

	float RoundStartTime = 0.f;

	// Local player controller
	UPROPERTY()
	AEXPlayerController* EXController = nullptr;

	UPROPERTY()
	AEXLevelRules* LevelRules = nullptr;

	virtual void CreateCurrentState(const FName& NewState, int32 NewStateDuration) {}

	void ChangeState(FEXMatchState* NewState);
	void OnStateChanged(const FString& StateName);


	virtual void BeginPlay() override;

	FEXMatchState* CurrentMatchState = nullptr;

private:
	UPROPERTY()
	TArray<AEXPlayerState*> EXPlayerArray;

	EGameMode Mode = EGameMode::Invalid;

	UPROPERTY()
	AEXGameModeBase* GMB = nullptr;

	bool bInitialized = false;
};
