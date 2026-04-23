// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXLaser.generated.h"

UCLASS()
class EX_API AEXLaser : public AActor
{
	GENERATED_BODY()
	
public:	
	AEXLaser();

	virtual void Tick(float DeltaTime) override;

	void SetTargetLocation(const FVector InTargetLocation);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Laser")
	float DPS = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Laser")
	float MovementSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Laser")
	float MinZ = -10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Laser")
	float MaxZ = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Laser")
	FDamageEvent DamageEvent;

	UPROPERTY(EditDefaultsOnly, Category = "Laser")
	float LifeSpan = 5.f;
private:

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Laser = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* Collision = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXLaserMovementComponent* Movement = nullptr;

};
