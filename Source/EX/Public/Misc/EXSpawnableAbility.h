// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXAbilityInventory.h"
#include "EXSpawnableAbility.generated.h"

class AEXPlacingAbility;
class AEXPlacedAbility;

/**
 * 
 */
UCLASS()
class EX_API UEXSpawnableAbility : public UEXAbilityInventory
{
	GENERATED_BODY()
	
public:
	UEXSpawnableAbility();

	// Start placing the ability
	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;

	// If the selected placement is valid, call the server function to spawn the ability
	virtual bool StopFire(EWeaponInput Mode, float WorldTimeOverride) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_PlaceAbility();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_SpawnAbility(const FVector Location, const FRotator Rotation);

	virtual bool CanFire(EWeaponInput Mode) const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<AEXPlacingAbility> TemporaryAbilityClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<AEXPlacedAbility> AbilityClass = nullptr;

	// Wireframe actor to show where the ability will be placed
	UPROPERTY()
	class AEXPlacingAbility* TemporaryAbility = nullptr;

	// Abilities that exist in the world
	UPROPERTY()
	TArray<AEXPlacedAbility*> SpawnedAbilities;

	virtual void Cancel() override;

	int32 MaxCount = 1;
};
