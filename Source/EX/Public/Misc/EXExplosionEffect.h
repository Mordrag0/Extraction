// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXTypes.h"
#include "EXExplosionEffect.generated.h"

class UParticleSystem;
class UPointLightComponent;
class USoundCue;

UCLASS(Abstract, Blueprintable)
class EX_API AEXExplosionEffect : public AActor
{
	GENERATED_BODY()

public:
	AEXExplosionEffect();

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	UParticleSystem* ExplosionFX;

	/** Returns ExplosionLight subobject **/
	FORCEINLINE UPointLightComponent* GetExplosionLight() const { return ExplosionLight; }
public:

	/** How long keep explosion light on? */
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float ExplosionLightFadeOut;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	USoundCue* ExplosionSound;

	// How far away can the impact be from the explosion to still draw the decal?
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float MaxImpactDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	struct FDecalData Decal;

	/** Surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	FHitResult SurfaceHit;

	/** Update fading light */
	virtual void Tick(float DeltaSeconds) override;

	float VolumeMultiplier = 1.f;

	UPROPERTY(BlueprintReadOnly, Category = "Effect")
	bool bComponentHit = false;
protected:
	float LightIntensity = 500.f;

	/** Spawn explosion */
	virtual void BeginPlay() override;

private:
	/** Explosion light */
	UPROPERTY(VisibleDefaultsOnly, Category = "Effect")
	UPointLightComponent* ExplosionLight;
};
