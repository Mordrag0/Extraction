// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXTypes.h"
#include "GameFramework/GameModeBase.h"
#include "EXAssaultCourseGame.generated.h"

class AEXCharacter;
class UTexture2D;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChangedSignature, int32, Score);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTimeUpdatedSignature, int32, DeciSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAssaultCourseResetSignature);

/**
 * 
 */
UCLASS()
class EX_API AEXAssaultCourseGame : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AEXAssaultCourseGame();

	void CalculateScore(float RunTime);

	UFUNCTION(BlueprintCallable, Category = "Assault course")
	virtual void ResetCourse();

	UFUNCTION(BlueprintCallable, Category = "Assault course")
	virtual void Start();

	UFUNCTION(BlueprintCallable, Category = "Assault course")
	virtual void Finish();

	UFUNCTION(BlueprintCallable, Category = "Assault course")
	virtual void AddScore(int32 InScore);

	UFUNCTION(BlueprintCallable, Category = "Assault course")
	virtual float GetTimeElapsed() const;

	UPROPERTY(BlueprintAssignable, Category = "Assault course")
	FScoreChangedSignature OnScoreChanged;
	UPROPERTY(BlueprintAssignable, Category = "Assault course")
	FTimeUpdatedSignature OnTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Assault course")
	FAssaultCourseResetSignature OnReset;

	UFUNCTION(BlueprintCallable, Category = "Assault course")
	bool IsActive() const { return bActive; }

	void SubmitScore(int32 InScore);

	void ReceiveGlobalStats(const FAssaultCourseStats& InGlobalStats);


	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual void StartToLeaveMap() override;

	void LoadMedals(const TMap<uint32, uint32>& InMedalInfo);

protected:

	UFUNCTION()
	void UpdateTimer();

	float StartTime = 0.f;
	bool bActive = false;
	int32 Score = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Assault course")
	int32 DefaultScore = 1000;
	UPROPERTY(EditDefaultsOnly, Category = "Assault course")
	float PenaltyPerSecond = 25.f;

	int32 PBScore = 0;

	FTimerHandle TimerHandle_RunTimer;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AssaultCourse")
	TSoftClassPtr<AEXCharacter> AssaultMercClass = nullptr;

	FAssaultCourseStats GlobalStats;

	TMap<uint32, uint32> MedalInfo; // Item ids of medals from lowest to highest and the score required to obtain them

	UPROPERTY()
	TMap<uint32, UTexture2D*> MedalIcons;

	UPROPERTY(EditDefaultsOnly, Category = "Menu")
	UDataTable* ItemTable = nullptr;
};
