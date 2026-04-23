// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXFireMode.h"
#include "EXFireModeAutomatic.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), Meta = (BlueprintSpawnableComponent))
class EX_API UEXFireModeAutomatic : public UEXFireMode
{
	GENERATED_BODY()

public:
	virtual bool Fire(float WorldTimeOverride) override;

	virtual void StopFire(float WorldTimeOverride) override;

	virtual bool CanFire() const override;

protected:
	virtual void FireComplete() override;
};
