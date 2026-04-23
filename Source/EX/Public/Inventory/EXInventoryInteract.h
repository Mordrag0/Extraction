// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXInventory.h"
#include "Misc/EXObjectiveTool.h"
#include "EXTypes.h"
#include "EXInventoryInteract.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXInventoryInteract : public UEXInventory, public IEXObjectiveTool
{
	GENERATED_BODY()
	
public:

	virtual bool Unequip() override;

	virtual EEngiTool GetEngiType() const override { return Tool; }

	virtual void EquipComplete() override;

	virtual void UnequipComplete() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Engi")
	EEngiTool Tool;

	UPROPERTY(EditDefaultsOnly, Category = "Engi")
	float DefaultProgressModifier = 1.f;

	virtual void BeginPlay() override;

};
