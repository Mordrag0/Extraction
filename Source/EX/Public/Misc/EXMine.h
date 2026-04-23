// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXAbility.h"
#include "EXMine.generated.h"

UCLASS()
class EX_API AEXMine : public AEXAbility
{
	GENERATED_BODY()
	
public:	
	AEXMine();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual UEXProjectileMovementComponent* GetProjectileMovementComponent() const override { return Movement; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void Trigger(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
	UFUNCTION()
	void Explode();

	virtual void PlayDeathEffects() override;

	UPROPERTY(EditDefaultsOnly, Category = "Mine")
	float Delay = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mine")
	float Damage = 150.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mine")
	float DamageRadius = 150.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mine")
	TSubclassOf<UDamageType> DamageType = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Mine")
	TSubclassOf<class AEXExplosionEffect> ExplosionEffectClass;

	bool bExploded = false;

	UFUNCTION()
	void Enable(const FHitResult& ImpactResult);

	FTimerHandle TimerHandle_Explode;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mine", Meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Mine", Meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collision = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Mine", Meta = (AllowPrivateAccess = "true"))
	class UEXProjectileMovementComponent* Movement = nullptr;
};
