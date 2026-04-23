// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeaponAction.h"
#include "EXFireMode.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXFireMode : public UEXWeaponAction
{
	GENERATED_BODY()
	
public:
	UEXFireMode();

	float GetSpreadTime(float WorldTimeOverride) const;

	virtual float GetSpread(float WorldTimeOverride) const override;

	virtual bool Fire(float WorldTimeOverride) override;

	virtual void Initialize(UEXWeapon* Weapon, EWeaponInput Input) override;

	virtual bool CanFire() const override;

	virtual void FinishEarly() override;

	virtual uint8 GetAmmoPerShot() const override { return AmmoPerShot; }

	virtual float GetTimeRemaining() const override;

protected:
	FTimerHandle TimerHandle_Fire;

	virtual void FireComplete() override;

	float LastShotWorldTime = -1.f;
	float SpreadTimeAfterLastShot = 0.f;

	// How far to the right we go on the curve with each shot
	UPROPERTY(EditDefaultsOnly, Category = "Fire Mode")
	float SpreadTimeAddedPerShot = .5f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire Mode")
	class UCurveFloat* Spread = nullptr;

	float MinSpreadTime = 0.f;
	float MaxSpreadTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	uint8 AmmoPerShot = 1;


};
