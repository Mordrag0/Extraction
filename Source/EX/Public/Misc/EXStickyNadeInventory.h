// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EXThrowableAbility.h"
#include "EXStickyNadeInventory.generated.h"

class UEXStickyDetonator;

/**
 * 
 */
UCLASS()
class EX_API UEXStickyNadeInventory : public UEXThrowableAbility
{
	GENERATED_BODY()

public:
	UEXStickyNadeInventory();

	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;

	virtual void Init(AEXAbility* SpawnedAbility) override;

	void SetDetonator(UEXStickyDetonator* InDetonator) { Detonator = InDetonator; }

	virtual void PreInitialize() override;

protected:

	UPROPERTY()
	UEXStickyDetonator* Detonator = nullptr;
};
