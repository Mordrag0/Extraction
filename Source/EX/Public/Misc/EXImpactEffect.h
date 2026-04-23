// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EXTypes.h"
#include "EXImpactEffect.generated.h"

class UParticleSystem;
class USoundCue;

UCLASS(Abstract, Blueprintable)
class EX_API AEXImpactEffect : public AActor
{
	GENERATED_BODY()

public:
	AEXImpactEffect();

	/** default impact FX used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* DefaultFX;

	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* BodyFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* HeadFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* LimbFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* ConcreteFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* DirtFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* WaterFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* MetalFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* GrassFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* GlassFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* WoodFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* AsphaltFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* BrickFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* BarkFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* Dirt2FX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* GravelFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* IceFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* MudFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* PlasticFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* RockFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* SandFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* SnowFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* SoilFX;
	UPROPERTY(EditDefaultsOnly, Category = "Visual") UParticleSystem* VegetationFX;
	
	/** default impact sound used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* DefaultSound;

	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* BodySound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* HeadSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* LimbSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* ConcreteSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* DirtSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* WaterSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* MetalSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* GrassSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* GlassSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* WoodSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* AsphaltSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* BrickSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* BarkSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* Dirt2Sound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* GravelSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* IceSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* MudSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* PlasticSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* RockSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* SandSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* SnowSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* SoilSound;
	UPROPERTY(EditDefaultsOnly, Category = Defaults) USoundCue* VegetationSound;


	/** default decal when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	struct FDecalData DefaultDecal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = Surface)
	FHitResult SurfaceHit;

	/** spawn effect */
	virtual void PostInitializeComponents() override;

protected:

	/** get FX for material type */
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** get sound for material type */
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

};
