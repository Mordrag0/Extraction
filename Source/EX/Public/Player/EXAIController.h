// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EXAIController.generated.h"

class AEXCharacter;

/**
 * 
 */
UCLASS()
class EX_API AEXAIController : public AAIController
{
	GENERATED_BODY()

public:

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool HasPath() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool CanFire() const;
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool IsFiring() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void Fire(float Time = 0.f);
	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopFire();

protected:

	bool bFiring = false;
	FTimerHandle TimerHandle_Fire;

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UPROPERTY()
	AEXCharacter* EXCharacter = nullptr;
};
