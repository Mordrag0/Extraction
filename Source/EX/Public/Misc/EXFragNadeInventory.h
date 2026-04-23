// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EXThrowableAbility.h"
#include "EXFragNadeInventory.generated.h"


/**
 * 
 */
UCLASS()
class EX_API UEXFragNadeInventory : public UEXThrowableAbility
{
	GENERATED_BODY()
	
public:
	UEXFragNadeInventory();

	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;

	virtual bool StopFire(EWeaponInput Mode, float WorldTimeOverride) override;

	bool CanUse(float Amount = 1.f) const override;

protected:
	virtual void Init(AEXAbility* SpawnedAbility) override;

	UFUNCTION()
	void MaxHold();

	virtual void Throw() override;

	UPROPERTY(EditDefaultsOnly, Category = "FragNade")
	float TimeToExplode = 3.f;

	float MaxHoldTime = 2.75f; // = TimeToExplode - TimeToThrow

	FTimerHandle TimerHandle_MaxHold;


	virtual void BeginPlay() override;

private:
	float TimeRemaining = 0.f;
};
