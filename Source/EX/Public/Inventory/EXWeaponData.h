// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXInventoryData.h"
#include "EXWeaponData.generated.h"

class UTexture2D;
class UEXWeapon;

/**
 * 
 */
UCLASS()
class EX_API UEXWeaponData : public UEXInventoryData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEXWeapon> WeaponClass;
};
