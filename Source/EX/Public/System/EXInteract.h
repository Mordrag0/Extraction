// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXInteractable.h"
#include "EXTypes.h"
#include "EXInteract.generated.h"

class UEXProgress;
class AEXPlayerController;
class AEXLevelRules;

DECLARE_DELEGATE_OneParam(FObjectiveActiveChanged, bool);

// Base class for objectives
UCLASS()
class EX_API AEXInteract : public AActor, public IEXInteractable
{
	GENERATED_BODY()
	
public:	
	AEXInteract();

	//~ Begin IEXInteractable Interface
	virtual bool CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const override;
	virtual bool StartInteract_Implementation(AEXCharacter* Player) override;
	virtual bool StopInteract_Implementation(AEXCharacter* Player) override;
	virtual float GetMovementMultiplier() const override { return InteractMovementMultiplier; }
	//~ End IEXInteractable Interface

	void Tick(float DeltaSeconds) override;

	virtual void ChangeIconVisibility(bool bVisible) {};

	// Start objective progress which can be delayed after calling StartInteract (because we might be equipping a tool first)
	virtual void StartProgress(AEXCharacter* Player) {};

	virtual void Reset() override;

	virtual void SetProgressComp(UEXProgress* InProgressComp);
	UEXProgress* GetProgressComp() const { return ProgressComp; }

	UFUNCTION()
	virtual void OnActivated();
	UFUNCTION()
	virtual void OnCompleted(AEXPlayerController* Player);
	UFUNCTION()
	virtual void ClearProgressComp();
	UFUNCTION()
	virtual void ProgressUpdated(float Value);
	UFUNCTION()
	virtual void OnProgressReset();

	AEXCharacter* GetInteractingPlayer() const;

	// For overtime checks
	void SetObjectiveActive(bool bInActive);
	FORCEINLINE bool GetObjectiveActive() const { return bObjectiveActive; }

	FObjectiveActiveChanged& GetObjectiveActiveChangedDelegate() { return PrimaryObjectiveActiveChanged; }

	void LevelStageChanged(int32 LevelStage);

	FORCEINLINE int32 GetStage() const { return Stage; }
	FORCEINLINE bool IsPrimary() const { return bPrimary; }

	virtual void Passed();
	virtual void SetActive();

	EInteractableState GetState() const { return State; }
	bool IsObjectiveActive() const { return State == EInteractableState::Active; }

	void StopInteraction();

	void SetObjectiveIdx(int32 InObjectiveIdx) { ObjectiveIdx = InObjectiveIdx; }

	virtual bool CanBeDefused() const { return false; }

	int32 GetRepeats() const;

	void SetCompletions(UEXProgress* InProgress, int32 Completions);

	EEngiTool GetRequiredTool() const { return RequiredTool; }

	void SetStage(int32 InStage) { Stage = InStage; }

protected:
	UPROPERTY(EditAnywhere, Category = "Objective")
	float InteractMovementMultiplier = 1.f;
	UPROPERTY(EditInstanceOnly, Category = "Objective")
	int32 Stage = 0;
	UPROPERTY(EditAnywhere, Category = "Objective")
	bool bNeedEyeContact = true;

	UPROPERTY(EditAnywhere, Category = "Objective")
	bool bPrimary = false;

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	EEngiTool RequiredTool = EEngiTool::None; 

	bool IsSomeoneFasterInteracting(const AEXCharacter* Player) const;


	virtual void BeginPlay() override;

	class AEXGameModeBase* GM = nullptr;
	class AEXGameStateBase* GS = nullptr;

	AEXCharacter* InteractingPlayer = nullptr;

	bool bOneInteractAtOnce = true;
	int32 NumCompletions = 0;

	UPROPERTY()
	bool bDamagable = false;

	UPROPERTY(Transient)
	EInteractableState State;
private:

	UPROPERTY()
	UEXProgress* ProgressComp = nullptr;
	bool bObjectiveActive = false; // While this is true, the game can be put into overtime
	FObjectiveActiveChanged PrimaryObjectiveActiveChanged;

	FDelegateHandle DelegateHandle_OnUpdate;
	FDelegateHandle DelegateHandle_OnCompleted;
	FDelegateHandle DelegateHandle_OnReset;

	// Local PC
	AEXPlayerController* EXController = nullptr;
	int32 ObjectiveIdx = -1;
	AEXLevelRules* LevelRules = nullptr;
};
