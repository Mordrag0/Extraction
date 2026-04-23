// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXAbilityInventory.h"
#include "EXStickyDetonator.generated.h"

class UEXStickyNadeInventory;
class AEXStickyNade;

/**
 * 
 */
UCLASS()
class EX_API UEXStickyDetonator : public UEXAbilityInventory
{
	GENERATED_BODY()
	
public:

	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;


	virtual void PreInitialize() override;

	void SetNadeInventory(UEXStickyNadeInventory* InNadeInventory) { NadeInventory = InNadeInventory; }

	void AddSticky(AEXStickyNade* InSticky) { Stickies.Add(InSticky); }

protected:

	UEXStickyNadeInventory* NadeInventory = nullptr;


	UPROPERTY()
	TArray<AEXStickyNade*> Stickies;
};
