// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "EXTargetPoint.generated.h"

UENUM(BlueprintType)
enum class TargetPointType : uint8
{
	Direction,
	Spawn,
};

/**
 * 
 */
UCLASS()
class EX_API AEXTargetPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Target point")
	TargetPointType Type;
};
