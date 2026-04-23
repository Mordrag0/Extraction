// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EXProjectileMovementComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, ClassGroup = (Custom), Meta = (BlueprintSpawnableComponent))
class EX_API UEXProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual void InitializeComponent() override;

	FORCEINLINE bool IsStopped() const { return bStopped; }

	void SetSticky(bool bInSticky);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void MoveInterpolationTarget(const FVector& NewLocation, const FRotator& NewRotation) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileMovement")
	bool bInheritVelocity = true;

	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

	UFUNCTION()
	void Stopped(const FHitResult& ImpactResult);

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileMovement")
	bool bSticky = false;

	// At which point of Hit normal Z it stops bouncing
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileMovement")
	float BounceCutOff = .8f;

	bool bStopped = false;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileMovement")
	bool bLockPitch = true;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileMovement")
	float MaxClientOffset = 10.f;

	virtual void TickInterpolation(float DeltaTime) override;

	UPROPERTY(ReplicatedUsing = OnRep_StuckHit)
	FHitResult StuckHit;

	UFUNCTION()
	void OnRep_StuckHit();

private:
	float MaxClientOffsetSquared = 0.f;

	FVector ClientTargetLocation;
	FRotator ClientTargetRotation;
};
