// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXGrenade.h"
#include "EXFragNade.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXFragNade : public AEXGrenade
{
	GENERATED_BODY()
	
public:
	AEXFragNade();

	void SetTimeToExplode(float TimeToExplode);
protected:

	FTimerHandle TimerHandle_Explode;
};
