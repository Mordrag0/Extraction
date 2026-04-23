// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "EXHomingMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXHomingMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	UEXHomingMovementComponent();

	void SetIsHoming(bool bInHoming);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetTarget(const FVector& InTarget);

	bool HasTarget() const;

	void ClearTarget();

	FVector LimitVelocity(const FVector& InVelocity);

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Homing")
	float MinVelocity = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Homing")
	float MaxVelocity = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Homing")
	float AccelerationAmount = 300.f;

	bool bHoming = false;

	FVector Target = FVector::ZeroVector;

private:

	const FVector ComputeAcceleration(const FVector InitialVelocity, float DeltaTime, bool bChangeDirection);
};
