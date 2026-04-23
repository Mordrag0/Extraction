// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../EXTypes.h"
#include "EXAbilityTracker.generated.h"

class AEXAbility;
class UEXInventoryComponent;
class AEXCharacter;

USTRUCT()
struct FCooldownInfo
{
	GENERATED_BODY()

	TMap<EAbility, float> Uses;
	float TimeSeconds;
};

/**
 * Class that keeps track for ability info that persists after the character dies and its inventory is deleted
 */
UCLASS()
class EX_API UEXAbilityTracker : public UObject
{
	GENERATED_BODY()
	
public:
	void OnCharacterDeath(AEXCharacter* Character);
	void OnCharacterSpawned(AEXCharacter* Character);

	void AddAbility(AActor* Ability);
	void DestroyAbilities();
protected:
	TMap<EMerc, FCooldownInfo> Cooldown;

	UPROPERTY()
	TArray<AActor*> Abilities;
};
