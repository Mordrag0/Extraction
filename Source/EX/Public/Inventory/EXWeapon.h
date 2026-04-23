// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXInventory.h"
#include "EXTypes.h"
#include "EX.h"
#include "EXWeapon.generated.h"

class UEXWeaponAction;
class UParticleSystemComponent;
class UAudioComponent;
class AEXImpactEffect;
class UCurveFloat;
class USoundCue;
class USoundAttenuation;
class UParticleSystem;
class UEXFireMode;
class UEXADSAction;
class UEXReloadAction;


/**
 * 
 */
UCLASS()
class EX_API UEXWeapon : public UEXInventory
{
	GENERATED_BODY()
	
public:
	UEXWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsInitialized() const override;

	virtual void BeginPlay() override;

	virtual void PreInitialize() override;

	virtual bool CanFire(EWeaponInput Mode) const override;
	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;
	virtual void FireComplete(EWeaponInput Mode) override;

	virtual bool StopFire(EWeaponInput Mode, float WorldTimeOverride) override;
	virtual bool ToggleOff(EWeaponInput Mode, float WorldTimeOverride);

	UFUNCTION()
	virtual void PerformShot();

	virtual bool FirePressed(EWeaponInput Mode) override;
	virtual bool FireReleased(EWeaponInput Mode) override;

	virtual bool Equip() override;
	virtual bool Unequip() override;

	virtual TArray<FSoftObjectPath> GetAssets() const override;

	virtual void EquipComplete() override;

	virtual void UnequipComplete() override;

	void AddRecoil();

	bool IsLowOnAmmo() const;
	bool IsAmmoFull() const;
	int32 AddAmmo(uint8 MagAmount);

	virtual bool Tick(float DeltaTime) override;

	virtual void LoadSettings() override;

	virtual float GetActionTimeRemaining(EWeaponState State) const override;

	virtual void FinishCurrentState(EWeaponState State) override;

public:
	virtual bool HasEnoughAmmo(uint8 Amount) const;
	virtual void ConsumeAmmo(uint8 Amount);

	void UpdateHUD();
protected:
	UFUNCTION()
	virtual void PlayFireEffects();
	// #EXTODORELIABLE
	UFUNCTION(NetMulticast, Unreliable, Category = "Effects")
	virtual void Multicast_Unreliable_PlayFireEffects();
	UFUNCTION()
	virtual void StopFireEffects();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleEffect = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FName MuzzleSocketName = FName("MuzzleSocket");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundCue* FireSound = nullptr; // #EXTODO soft pointer


	float Spread = 0.f;

	float FireModeSpreadModifier = 1.f; // For example ADS will be 0
	float MovementSpreadModifier = 1.f;

	/** Recalculate weapon spread based on the default spread, character spread and firemode spread */
	void RecalculateSpread(float WorldTimeOverride);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UCurveFloat* RecoilX = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UCurveFloat* RecoilY = nullptr;
		
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RecoilMultiplierX = .3f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RecoilMultiplierY = 1.f;

	bool bRecoil = false;
	float TimeSinceLastShot = 0.f;
	float TotalShotRecoilX;
	float TotalShotRecoilY;
	bool bRecoilXDirection = false;

	float RecoilTime = 0.f;

public:
	void SetFireModeSpreadModifier(float Val);

	void SetMovementSpreadModifier(float Val);

	void OnOutOfAmmo();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bReloadOnFireNoAmmo = true;

	UFUNCTION()
	virtual void OnRep_Actions();

	// Spawn effects for impact
	void SpawnImpactEffect(const FHitResult& Impact);

	// Impact effects
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<AEXImpactEffect> ImpactEffect; 

	FHitResult PerformFXTrace(FVector StartTrace, FVector EndTrace) const;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TMap<EWeaponInput, TSubclassOf<UEXWeaponAction>> ActionClassList;
public:
	TArray<UEXWeaponAction*> GetReplicatedActions() const { return ReplicatedActions; }

	TMap<EWeaponInput, UEXWeaponAction*> GetActions() const { return Actions; }

	UEXFireMode* GetActiveFireMode() const;
	UEXReloadAction* GetReloadAction() const;
	UEXADSAction* GetADSAction() const;

	virtual bool CanSprint() const override;

	virtual void CancelReload() override;
protected:
	UPROPERTY(ReplicatedUsing = OnRep_Actions)
	TArray<UEXWeaponAction*> ReplicatedActions;

	UPROPERTY()
	TMap<EWeaponInput, UEXWeaponAction*> Actions;

	UPROPERTY()
	UParticleSystemComponent* MuzzleParticleComp = nullptr;
	UPROPERTY()
	UAudioComponent* FireSoundComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundAttenuation* AttenuationSettings = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundAttenuation* AttenuationSettings3P = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundConcurrency* ConcurrencySettings = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundConcurrency* ConcurrencySettings3P = nullptr;

};
