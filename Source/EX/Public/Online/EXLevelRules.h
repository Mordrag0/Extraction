// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "EXTypes.h"
#include "EXLevelRules.generated.h"

class AEXTeam;
class AEXCharacter;
class AEXPlayerController;
class AEXInteract;
class AEXPlayerState;
class AEXGameStateBase;
class AEXSpawnArea;

/**
 * 
 */
UCLASS(Blueprintable)
class EX_API AEXLevelRules : public AInfo
{
	GENERATED_BODY()

public:
	AEXLevelRules();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetGameState(AEXGameStateBase* InGS) { GSB = InGS; }

	// Set stage to 0, so no objectives can be interacted with until round is started and AdvanceStage is called
	void ResetStage();
	// Increases stage by 1, changes which objectives are now active and ends the round if the last stage is over
	void AdvanceStage();

	void PostInitializeComponents() override;

	TArray<AEXInteract*> GetObjectives() const { return Objectives; }

	virtual void Reset() override;

	FORCEINLINE int32 GetStage() const { return Stage; }
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objective")
	int32 NumStages = 1;

	void OnStageChanged();

	// #EXTODORELIABLE
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Reliable_OnStageChanged(int32 InStage);

	void UpdatePrimaryObjectiveOnHUD(int32 NumActiveObjectives, TArray<bool> CanBeDefused);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void InitInteractable();

	UFUNCTION()
	void AddInteractable(AActor* Ability);
	UFUNCTION()
	void RemoveInteractable(AActor* Ability);

	UFUNCTION(BlueprintCallable, Category = "Interact")
	TArray<AActor*> GetInteractableActors() const { return InteractableActors; }

protected:

	// Actors that we iterate to find the one that the player can interact with
	UPROPERTY(BlueprintReadOnly, Category = "Interact")
	TArray<AActor*> InteractableActors;
	UPROPERTY()
	TArray<AEXInteract*> Objectives;

public:

	UFUNCTION()
	void ObjectiveCompleted(AEXInteract* Objectiveg);

public:

	bool OvertimeCheck() const;

	TArray<AEXSpawnArea*> GetAttackerSpawns() const { return AttackerSpawns; }
	TArray<AEXSpawnArea*> GetDefenderSpawns() const { return DefenderSpawns; }

#pragma region Vote
public:
	void VoteKick(AEXPlayerState* Instigator, AEXPlayerState* Target);

	int32 GetNumVoters(EVote VoteType, AEXTeam* Team) const;
	int32 GetVoteDuration() const { return CurrentVote.VoteDuration; }

	void AddVote(AEXPlayerState* EXPS, uint8 Option);
protected:
	bool CanVoteKick(AEXPlayerState* VoteInstigator, AEXPlayerState* Target) const;

	void StartVote(EVote VoteType, AEXPlayerState* VoteInstigator, int32 Option);
	void EndVote();

	UPROPERTY(ReplicatedUsing = OnRep_CurrentVote)
	FVoteInfo CurrentVote;

	UFUNCTION()
	void OnRep_CurrentVote(const FVoteInfo& PreviousVote);

	FTimerHandle TimerHandle_Vote;

#pragma endregion Vote

protected:
	UPROPERTY(EditInstanceOnly, Category = "Spawns")
	TArray<AEXSpawnArea*> AttackerSpawns;
	UPROPERTY(EditInstanceOnly, Category = "Spawns")
	TArray<AEXSpawnArea*> DefenderSpawns;
private:

	// #OVERTIME
	UFUNCTION()
	void PrimaryObjectiveActiveChanged(bool bActive);

	int32 NumActivePrimaryObjectives = 0;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Stage)
	int32 Stage = 0;

	UFUNCTION()
	void OnRep_Stage();

	UPROPERTY()
	class AEXGameModeBase* GMB = nullptr;
	UPROPERTY()
	class AEXGameStateBase* GSB = nullptr;
	UPROPERTY()
	AEXPlayerController* EXController = nullptr;

};
