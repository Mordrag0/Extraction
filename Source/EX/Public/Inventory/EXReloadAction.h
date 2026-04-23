// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeaponAction.h"
#include "EXReloadAction.generated.h"

class USoundCue;

/**
 * 
 */
UCLASS()
class EX_API UEXReloadAction : public UEXWeaponAction
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(UEXWeapon* Weapon, EWeaponInput Type) override;

	virtual bool Fire(float WorldTimeOverride) override;

	virtual void StopFire(float WorldTimeOverride) override;
	virtual void ToggleOff(float WorldTimeOverride) override;

	virtual void Cancel(float WorldTimeOverride) override;

	virtual void FinishEarly() override;

	bool CanFire() const override;

	uint8 GetMagAmmo() const { return MagAmmo; }
	uint8 GetTotalAmmo() const { return MagAmmo + SpareAmmo; }
	uint8 GetSpareAmmo() const { return SpareAmmo; }
	uint8 GetMaxAmmo() const { return MaxAmmo; }

	int32 AddAmmo(int32 Mags);

	bool IsLowOnAmmo() const;
	bool IsMagLowOnAmmo() const;
	bool IsAmmoFull() const;
	void ConsumeAmmo(uint8 Amount);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetTimeRemaining() const override;

protected:
	void OnMagLowOnAmmo();

	UFUNCTION()
	void ReloadComplete();

	FTimerHandle TimerHandle_Reload;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	uint8 MaxMagAmmo = 0;
	UPROPERTY()
	uint8 MagAmmo = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	uint8 MaxAmmo = 0;
	UPROPERTY(ReplicatedUsing = OnRep_SpareAmmo)
	uint8 SpareAmmo = 0;

	UFUNCTION()
	void OnRep_SpareAmmo();

	// At what value of ammo do we call OnLowAmmo()
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	uint8 LowAmmo = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundCue* LowAmmoBeep = nullptr; // #EXTODO soft pointer

};
