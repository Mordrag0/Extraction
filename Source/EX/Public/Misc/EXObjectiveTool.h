// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EXObjectiveTool.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEXObjectiveTool : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class EX_API IEXObjectiveTool
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION()
	virtual float GetProgressModifier() const { return ProgressModifier; }
	UFUNCTION()
	virtual void SetProgressModifier(float Modifier) { ProgressModifier = Modifier; }

private:
	float ProgressModifier = 1.f;
};
