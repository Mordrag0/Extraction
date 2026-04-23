// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXAbility.h"
#include "EXJumpPad.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UEXProjectileMovementComponent;

UCLASS()
class EX_API AEXJumpPad : public AEXAbility
{
	GENERATED_BODY()
	
public:	
	AEXJumpPad();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual UEXProjectileMovementComponent* GetProjectileMovementComponent() const override { return Movement; }

protected:
	UFUNCTION()
	void Launch(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float ForwardLaunchMin = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UpwardLaunchMin = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float ForwardLaunchMax = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UpwardLaunchMax = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float MaxHP = 100.f;

	float HP = 0.f;

	virtual void BeginPlay() override;

	virtual void Kill();
	UFUNCTION()
	void Enable(const FHitResult& ImpactResult);

private:
	bool bEnabled = false;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoostCollisionComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components", Meta = (AllowPrivateAccess = "true"))
	class UEXProjectileMovementComponent* Movement = nullptr;

};
