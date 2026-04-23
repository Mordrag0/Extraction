// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXMeleeWeapon.h"
#include "Misc/EXObjectiveTool.h"
#include "EXMilkJug.generated.h"

class AEXCarryableObjective;

/**
 * 
 */
UCLASS()
class EX_API UEXMilkJug : public UEXMeleeWeapon, public IEXObjectiveTool
{
	GENERATED_BODY()
	
public:
	UEXMilkJug();

	void SetJugActor(AEXCarryableObjective* MilkJug);

	FORCEINLINE AEXCarryableObjective* GetMilkJugActor() const { return MilkJugActor; }

	virtual bool Unequip() override;

	virtual void UnequipComplete() override;

	virtual void EquipComplete() override;


	virtual EEngiTool GetEngiType() const override { return EEngiTool::Deliver; }


	virtual bool CanEquip() const override;

protected:

	UPROPERTY()
	AEXCarryableObjective* MilkJugActor = nullptr;
};
