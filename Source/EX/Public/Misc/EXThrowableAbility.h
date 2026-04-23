// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXAbilityInventory.h"
#include "EXThrowableAbility.generated.h"

class AEXAbility;

/**
 * 
 */
UCLASS()
class EX_API UEXThrowableAbility : public UEXAbilityInventory
{
	GENERATED_BODY()

public:
	UEXThrowableAbility();

	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;

	virtual bool StopFire(EWeaponInput Mode, float WorldTimeOverride) override;

	virtual bool CanFire(EWeaponInput Mode) const override;

	virtual bool CanUse(float Amount = 1.f) const override;

protected:
	// Initialize the thrown ability, called during the spawning
	virtual void Init(AEXAbility* SpawnedAbility);

	virtual void Throw();

	virtual FTransform GetSpawnTransform() const;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<AEXAbility> AbilityClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Ability", Meta = (ToolTip = "How long after going in Firing state will the ability be thrown"))
	float TimeToThrow = .5f;

	void Cancel() override;

	FTimerHandle TimerHandle_Throw;

	bool bCanRecover = false;

	bool bCanThrow = false;
};
