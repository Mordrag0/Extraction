// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeapon.h"
#include "EXGrenadeLauncher.generated.h"

class AEXGrenade;

/**
 * 
 */
UCLASS()
class EX_API UEXGrenadeLauncher : public UEXWeapon
{
	GENERATED_BODY()
	
public:
	UEXGrenadeLauncher();

	virtual void PerformShot() override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_Reliable_PerformShot(const FVector Loc, const FRotator Rot);


	virtual bool CanFire(EWeaponInput Mode) const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FVector ProjectileSpawnOffset = FVector(100, 0, 0);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AEXGrenade> ProjectileClass;
};
