// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXFireMode.h"
#include "EXFireModeHold.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXFireModeHold : public UEXFireMode
{
	GENERATED_BODY()
	
public:
	UEXFireModeHold();

	virtual bool Fire(float WorldTimeOverride) override;

	virtual void StopFire(float WorldTimeOverride) override;

};
