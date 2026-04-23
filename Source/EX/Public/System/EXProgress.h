// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Controller.h"
#include "EXTypes.h"
#include "EXProgress.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnProgressCompleted, AEXPlayerController*);
DECLARE_MULTICAST_DELEGATE(FOnProgressReset);
DECLARE_MULTICAST_DELEGATE_OneParam(FProgressUpdate, float);

class AEXCharacter;

UCLASS( BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EX_API UEXProgress : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEXProgress();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	// Returns how much of the progress has been done (between 0 and 1).
	UFUNCTION(BlueprintCallable, Meta = (ShortToolTip = "Progress / Goal"), Category = "Progress")
	FORCEINLINE float GetRatio() const { return FMath::Min(Progress / Goal, 1.f); }
	UFUNCTION(BlueprintCallable, Meta = (ShortToolTip = "Time remaining in seconds"), Category = "Progress")
	FORCEINLINE float GetTimeRemaining() const { return (Goal - Progress) / ProgressModifier; }

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void ResetProgress();

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void StartProgress(AEXPlayerController* InPlayer, float InProgressModifier = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void StopProgress(AEXPlayerController* InPlayer);

	FProgressUpdate ProgressUpdate;
	FOnProgressCompleted OnCompleted;
	FOnProgressReset OnReset;

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void SetGoal(float InGoal);
	void SetCompletions(int32 InCompletions);

	void SetResetOnStop(bool bVal) { bResetOnStop = bVal; }
	bool CanBeReset() const { return bResetOnStop; }

	UFUNCTION()
	FORCEINLINE float GetProgress() const { return Progress; }

	FORCEINLINE bool IsProgressing() const { return bProgressing; }

	void SetCharacterOwner(AEXCharacter* InCharacter) { EXCharacterOwner = InCharacter; }

	void AddProgressInstant(AEXPlayerController* InPlayer, float DeltaProgress);

	float GetGoal() const { return Goal; }

	FORCEINLINE int32 GetCompletions() const { return Completions; }
	FORCEINLINE int32 GetRepeats() const { return Repeats; }
protected:

	void UpdateHUD();

	void AddProgress(AEXPlayerController* InPlayer, float DeltaProgress);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Progress")
	float Progress = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Progress)
	uint16 ReplicatedProgress = 0;

	void SetProgress(float InProgress);

	UFUNCTION()
	void OnRep_Progress();
	UFUNCTION()
	void OnRep_Goal();
	UFUNCTION()
	void OnRep_Completions();

	UPROPERTY(Transient)
	bool bProgressing = false;

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_Goal, BlueprintReadOnly, Category = "Progress")
	float Goal = 10.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Completions)
	int32 Completions = 0;
	UPROPERTY(EditAnywhere, Category = "Progress")
	int32 Repeats = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Progress")
	bool bResetOnStop = true;

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void Complete(AEXPlayerController* InPlayer);

	float ProgressModifier = 1.f;

	UPROPERTY()
	AEXPlayerController* Player = nullptr;

	// If the EXProgress is a member of EXCharacter then this is the owner
	UPROPERTY()
	AEXCharacter* EXCharacterOwner = nullptr;

	UPROPERTY(EditAnywhere, Category = "Progress")
	float TotalExpForCompletion = 0.f;

	float CurrentRationStartedOn = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Progress")
	EScoreType ScoreType = EScoreType::Objective;

	int32 Stage = 0;
	class AEXLevelRules* LevelRules = nullptr;
	class AEXInteract* Objective = nullptr;
};
