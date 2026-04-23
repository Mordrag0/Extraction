// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXFireMode.h"
#include "EXMeleeFireMode.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXMeleeFireMode : public UEXFireMode
{
	GENERATED_BODY()

public:
	virtual bool Fire(float WorldTimeOverride) override;


	virtual bool CanFire() const override;

protected:
	virtual void FireComplete() override;
};
