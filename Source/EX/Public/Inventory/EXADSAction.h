// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeaponAction.h"
#include "EXADSAction.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXADSAction : public UEXWeaponAction
{
	GENERATED_BODY()
	
public:

	virtual bool Fire(float WorldTimeOverride) override;

	virtual void StopFire(float WorldTimeOverride) override;

	virtual void Initialize(UEXWeapon* Weapon, EWeaponInput Type) override;

	void StopADS();
protected:
	void StartADS();

	UFUNCTION()
	void InitPCM();

	UPROPERTY(EditDefaultsOnly, Category = "FireMode")
	float ADS_FOV = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireMode")
	bool bScope = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireMode")
	float SpreadModifier = 0.f;

	class AEXPlayerCameraManager* PCM = nullptr;

private:
	bool bADSing = false;
};
