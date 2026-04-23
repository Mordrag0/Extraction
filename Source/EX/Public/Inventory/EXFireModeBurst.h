// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXFireMode.h"
#include "EXFireModeBurst.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXFireModeBurst : public UEXFireMode
{
	GENERATED_BODY()
	
public:
	virtual bool Fire(float WorldTimeOverride) override;

	virtual void StopFire(float WorldTimeOverride) override;

	virtual bool CanFire() const override;

	void FinishEarly() override;

protected:
	virtual void FireComplete() override;

	// If firing fails we call this to clear the burst, so that once we can fire again, we start a new burst
	void ResetBurst();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_ResetBusrst();

	UPROPERTY(EditDefaultsOnly, Category = "FireMode")
	uint8 ShotsPerBurst = 3;

	uint8 RemainingBurst = 0;

	UPROPERTY(EditDefaultsOnly, Category = "FireMode")
	float TimeBetweenBursts = 1.f;

	UFUNCTION()
	void BurstComplete();

	FTimerHandle TimerHandle_Burst;
};
