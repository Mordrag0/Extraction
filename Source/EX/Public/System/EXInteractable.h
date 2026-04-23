// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EXTypes.h"
#include "EXInteractable.generated.h"

class AEXCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UEXInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EX_API IEXInteractable // #Interactable
{
	GENERATED_BODY()

public:

	// Target is the actor that the player is looking at
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	bool CanInteract(const AEXCharacter* Player, bool bLookAt) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	bool StartInteract(AEXCharacter* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interact")
	bool StopInteract(AEXCharacter* Player);

	UFUNCTION()
	virtual float GetMovementMultiplier() const { return 1.f; }
	// If true we call StopInteract immediately after StartInteract 
	// StopInteract() shouldn't do anything in this case, except clear some values
	UFUNCTION()
	virtual bool TapInteract() const { return false; }
};
