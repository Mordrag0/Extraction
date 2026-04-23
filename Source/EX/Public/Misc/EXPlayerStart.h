// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "EXPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	FORCEINLINE bool IsEnabled() const { return bEnabled; }

protected:
	UPROPERTY(EditAnywhere, Category = "EXPlayerStart")
	bool bEnabled = true;
};
