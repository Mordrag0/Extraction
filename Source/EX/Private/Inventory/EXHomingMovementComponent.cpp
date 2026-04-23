// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXHomingMovementComponent.h"

UEXHomingMovementComponent::UEXHomingMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetIsReplicatedByDefault(false);
}

void UEXHomingMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() != ROLE_Authority)
	{
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
}

const FVector UEXHomingMovementComponent::ComputeAcceleration(const FVector InitialVelocity, float DeltaTime, bool bChangeDirection)
{
	FVector AccelerationDir;
	if (bChangeDirection)
	{
		AccelerationDir = Target - UpdatedComponent->GetComponentLocation();
	}
	else
	{
		AccelerationDir = InitialVelocity;
	}
	AccelerationDir.Normalize();
	return AccelerationDir * DeltaTime * AccelerationAmount;
}

void UEXHomingMovementComponent::SetIsHoming(bool bInHoming)
{
	bHoming = bInHoming;
	PrimaryComponentTick.SetTickFunctionEnable(bHoming);
}

void UEXHomingMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HasTarget())
	{
		// v = v0 + a*t
		const FVector InitialVelocity = UpdatedComponent->GetComponentRotation().RotateVector(Velocity);
		const FVector Acceleration = ComputeAcceleration(InitialVelocity, DeltaTime, true);
		FVector NewVelocity = InitialVelocity + (Acceleration * DeltaTime);
		Velocity = LimitVelocity(NewVelocity);
		UpdatedComponent->SetWorldRotation(Velocity.Rotation());
		
		// Compare direction
		if (FVector::CrossProduct(Velocity, (Target - UpdatedComponent->GetComponentLocation())).IsNearlyZero())
		{
			ClearTarget();
		}
	}
	else
	{
		const FVector InitialVelocity = Velocity;
		const FVector Acceleration = ComputeAcceleration(InitialVelocity, DeltaTime, false);
		FVector NewVelocity = InitialVelocity + (Acceleration * DeltaTime);
		Velocity = LimitVelocity(NewVelocity);
		MoveUpdatedComponent(Velocity * DeltaTime, Velocity.Rotation(), false, nullptr, ETeleportType::None);
	}
}

void UEXHomingMovementComponent::SetTarget(const FVector& InTarget)
{
	Target = InTarget;
}

bool UEXHomingMovementComponent::HasTarget() const
{
	return !Target.IsNearlyZero();
}

void UEXHomingMovementComponent::ClearTarget()
{
	Target = FVector::ZeroVector;
}

FVector UEXHomingMovementComponent::LimitVelocity(const FVector& InVelocity)
{
	return InVelocity.GetClampedToSize(MinVelocity, MaxVelocity);
}
