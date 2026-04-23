// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXFireMode.h"
#include "EXFireModeCharge.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXFireModeCharge : public UEXFireMode
{
	GENERATED_BODY()
	
public:
	virtual bool Fire(float WorldTimeOverride) override;

	virtual void StopFire(float WorldTimeOverride) override;

	virtual bool CanFire() const override;

protected:
	virtual void FireComplete() override;

	UPROPERTY(EditDefaultsOnly, Category = "FireMode")
	float MinCharge = .2f;

};
