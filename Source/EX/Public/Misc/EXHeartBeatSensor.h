// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXAbility.h"
#include "EXHeartBeatSensor.generated.h"

UCLASS()
class EX_API AEXHeartBeatSensor : public AEXAbility
{
	GENERATED_BODY()
	
public:	
	AEXHeartBeatSensor();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual UEXProjectileMovementComponent* GetProjectileMovementComponent() const override { return Movement; }

protected:
	FTimerHandle TimerHandle_Spotting;

	UPROPERTY(EditDefaultsOnly, Category = "HeartBeatSensor")
	int32 NumSpots = 3;

	UPROPERTY(EditDefaultsOnly, Category = "HeartBeatSensor")
	float InitialDelay = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "HeartBeatSensor")
	float TimeBetweenSpots = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "HeartBeatSensor")
	float SpottingDuration = 3.f;

	UFUNCTION()
	void OnMovementStopped(const FHitResult& ImpactResult);

	UFUNCTION()
	void SpotNearbyEnemies();

	UPROPERTY()
	class AEXCharacter* EXCharacter = nullptr;

	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "HeartBeatSensor", Meta = (AllowPrivateAccess = "true"))
	class UEXProjectileMovementComponent* Movement = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "HeartBeatSensor", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "HeartBeatSensor", Meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collision = nullptr;


};
