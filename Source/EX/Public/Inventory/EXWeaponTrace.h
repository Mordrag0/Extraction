// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeapon.h"
#include "EXWeaponTrace.generated.h"

class AEXImpactEffect;

/**
 * 
 */
UCLASS()
class EX_API UEXWeaponTrace : public UEXWeapon
{
	GENERATED_BODY()

public:
	//~ Begin UInventory Interface
	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;
	//~ End UInventory Interface

	virtual void PerformShot() override;

	virtual void PerformTrace(float Charge = 1.f);

	UFUNCTION(Server, Unreliable, WithValidation)
	virtual void Server_Unreliable_TraceHit(const FHitResult& HitResult, float ShotCharge);

	UFUNCTION(Server, Unreliable)
	virtual void Server_Unreliable_TraceNoHit(const FVector_NetQuantize TraceEnd);

	virtual bool ValidateTrace(const FHitResult& HitResult) const;

protected:
	virtual void TraceHit(const FHitResult& HitResult, float ShotCharge);
	virtual void TraceNoHit(const FVector_NetQuantize TraceEnd);

	UFUNCTION()
	virtual void PlayTraceEffects(const FVector HitLocation);

	// #EXTODORELIABLE
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_PlayTraceEffects(const FVector HitLocation);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_SpawnImpactEffect(const FHitResult HitLocation);

	UPROPERTY()
	uint8 FireValidated = 0;  // #EXTODO
	UPROPERTY()
	float MaxOriginTolerance = 400.f;
	UPROPERTY()
 	float MaxTargetTolerance = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage = 10.f;	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MinRangeDamage = 5.f;	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class UDamageType> DamageType = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxTraceRange = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MinRange = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxRange = 7000.f;


	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	class UParticleSystem* TraceEffect = nullptr;

	FVector GetTraceDirection(const FRotator TracViewDirectioneDirection) const;
};
