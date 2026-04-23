// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EXPlacedAbility.h"
#include "EXTurret.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXTurret : public AEXPlacedAbility
{
	GENERATED_BODY()

public:
	AEXTurret();

	virtual void Tick(float DeltaSeconds) override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:

	UPROPERTY(Replicated)
	class AEXCharacter* Target = nullptr;


	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float Damage = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	bool bGib = false;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float SpottingDistance = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float ForgettingDistance = 1300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float AimHeight = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float BarrelHeight = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float Delay = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	float FireRate = 0.2f;

	float SpottingDistanceSq = 0.f;
	float ForgettingDistanceSq = 0.f;

	UFUNCTION()
	void TargetAquired(AEXCharacter* Player);

	UFUNCTION()
	void TargetLost();

	FTimerHandle TimerHandle_Fire;

	UFUNCTION()
	virtual void Fire();

	UFUNCTION()
	bool CanSeeTarget(AEXCharacter* InTarget, FHitResult& OutHit);

	// #EXTODORELIABLE
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_PlayFireEffects(const FVector HitLocation);

	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	class UParticleSystem* MuzzleEffect = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	class UParticleSystem* TraceEffect = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	class USoundCue* FireSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	TSubclassOf<class UDamageType> DamageType = nullptr;

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UPoseableMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CollisionComp = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USphereComponent* EnemySearchArea = nullptr;

};
