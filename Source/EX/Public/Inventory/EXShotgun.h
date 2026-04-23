// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeaponTrace.h"
#include "EXShotgun.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXShotgun : public UEXWeaponTrace
{
	GENERATED_BODY()
	
public:
	virtual void PerformShot() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	uint8 PelletsPerShot = 6.f;
};
