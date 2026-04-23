// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXAbilityInventory.h"
#include "EXSelfRevive.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXSelfRevive : public UEXAbilityInventory
{
	GENERATED_BODY()
	
public:
	UEXSelfRevive();

	bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;

	bool StopFire(EWeaponInput Mode, float WorldTimeOverride) override;

};
