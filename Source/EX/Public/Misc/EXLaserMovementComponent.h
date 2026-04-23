// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "EXLaserMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXLaserMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetTargetLocation(const FVector Target) { TargetLocation = Target; }

protected:
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float Speed = 10.f;
};
