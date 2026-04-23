// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeaponTrace.h"
#include "EXWeaponTraceCharge.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXWeaponTraceCharge : public UEXWeaponTrace
{
	GENERATED_BODY()

public:
	UEXWeaponTraceCharge();

	virtual bool CanFire(EWeaponInput Mode) const override;

	virtual bool StopFire(EWeaponInput Mode, float WorldTimeOverride) override;

	virtual void Overheat();

	virtual void OverheatComplete();

	virtual void BeginPlay() override;

	virtual bool Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ChargeMax = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BatteryMax = -1.f;

	float Charge = 0.f;
	float Battery = 0.f;
	bool bOverheated = false;

	FTimerHandle TimerHandle_Overheat;
};
