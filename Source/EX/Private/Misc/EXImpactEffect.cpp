// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXImpactEffect.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "EXTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXPersistentUser.h"
#include "Player/EXLocalPlayer.h"
#include "System/EXGameplayStatics.h"

AEXImpactEffect::AEXImpactEffect()
{
	SetAutoDestroyWhenFinished(true);
}

void AEXImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	// Show particles
	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	// Play sound
	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), UEXGameplayStatics::GetMasterVolume());
	}

	if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(1.0f, DefaultDecal.DecalSize, DefaultDecal.DecalSize),
											 SurfaceHit.Component.Get(), SurfaceHit.BoneName,
											 SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
											 DefaultDecal.LifeSpan);
	}
}

UParticleSystem* AEXImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = nullptr;

	switch (SurfaceType)
	{
		case SURFACE_Default: ImpactFX = DefaultFX; break;
		case SURFACE_Body: ImpactFX = BodyFX; break;
		case SURFACE_Head: ImpactFX = HeadFX; break;
		case SURFACE_Limb: ImpactFX = LimbFX; break;
		case SURFACE_Concrete: ImpactFX = ConcreteFX; break;
		case SURFACE_Dirt: ImpactFX = DirtFX; break;
		case SURFACE_Water: ImpactFX = WaterFX; break;
		case SURFACE_Metal: ImpactFX = MetalFX; break;
		case SURFACE_Grass: ImpactFX = GrassFX; break;
		case SURFACE_Glass: ImpactFX = GlassFX; break;
		case SURFACE_Wood: ImpactFX = WoodFX; break;
		case SURFACE_Asphalt: ImpactFX = AsphaltFX; break;
		case SURFACE_Brick: ImpactFX = BrickFX; break;
		case SURFACE_Bark: ImpactFX = BarkFX; break;
		case SURFACE_Dirt2: ImpactFX = Dirt2FX; break;
		case SURFACE_Gravel: ImpactFX = GravelFX; break;
		case SURFACE_Ice: ImpactFX = IceFX; break;
		case SURFACE_Mud: ImpactFX = MudFX; break;
		case SURFACE_Plastic: ImpactFX = PlasticFX; break;
		case SURFACE_Rock: ImpactFX = RockFX; break;
		case SURFACE_Sand: ImpactFX = SandFX; break;
		case SURFACE_Snow: ImpactFX = SnowFX; break;
		case SURFACE_Soil: ImpactFX = SoilFX; break;
		default: ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* AEXImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* ImpactSound = nullptr;

	switch (SurfaceType)
	{
		case SURFACE_Default: ImpactSound = DefaultSound; break;
		case SURFACE_Body: ImpactSound = BodySound; break;
		case SURFACE_Head: ImpactSound = HeadSound; break;
		case SURFACE_Limb: ImpactSound = LimbSound; break;
		case SURFACE_Concrete: ImpactSound = ConcreteSound; break;
		case SURFACE_Dirt: ImpactSound = DirtSound; break;
		case SURFACE_Water: ImpactSound = WaterSound; break;
		case SURFACE_Metal: ImpactSound = MetalSound; break;
		case SURFACE_Grass: ImpactSound = GrassSound; break;
		case SURFACE_Glass: ImpactSound = GlassSound; break;
		case SURFACE_Wood: ImpactSound = WoodSound; break;
		case SURFACE_Asphalt: ImpactSound = AsphaltSound; break;
		case SURFACE_Brick: ImpactSound = BrickSound; break;
		case SURFACE_Bark: ImpactSound = BarkSound; break;
		case SURFACE_Dirt2: ImpactSound = Dirt2Sound; break;
		case SURFACE_Gravel: ImpactSound = GravelSound; break;
		case SURFACE_Ice: ImpactSound = IceSound; break;
		case SURFACE_Mud: ImpactSound = MudSound; break;
		case SURFACE_Plastic: ImpactSound = PlasticSound; break;
		case SURFACE_Rock: ImpactSound = RockSound; break;
		case SURFACE_Sand: ImpactSound = SandSound; break;
		case SURFACE_Snow: ImpactSound = SnowSound; break;
		case SURFACE_Soil: ImpactSound = SoilSound; break;
		default: ImpactSound = DefaultSound; break;
	}

	return ImpactSound;
}

