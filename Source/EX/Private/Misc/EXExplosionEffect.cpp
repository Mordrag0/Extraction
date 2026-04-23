// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXExplosionEffect.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EXTypes.h"
#include "Sound/SoundCue.h"

AEXExplosionEffect::AEXExplosionEffect()
{
	PrimaryActorTick.bCanEverTick = true;

	ExplosionLight = CreateDefaultSubobject<UPointLightComponent>("ExplosionLight");
	RootComponent = ExplosionLight;
	ExplosionLight->AttenuationRadius = 400.0;
	ExplosionLight->bUseInverseSquaredFalloff = false;
	ExplosionLight->LightColor = FColor(255, 185, 35);
	ExplosionLight->CastShadows = false;
	ExplosionLight->SetVisibility(true);

	ExplosionLightFadeOut = 0.2f;
}

void AEXExplosionEffect::BeginPlay()
{
	Super::BeginPlay();

	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation());
	}

	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), VolumeMultiplier);
	}

	if (bComponentHit)
	{
		const float ImpactDistance = FVector::Distance(GetActorLocation(), SurfaceHit.ImpactPoint);
		float DecalScale = 1 - (ImpactDistance / MaxImpactDistance);
		if (Decal.DecalMaterial && (DecalScale > 0.f))
		{
			const FVector& Normal = SurfaceHit.Normal;
			const float Pitch = Normal.Z * 90.f;
			const float Yaw = Normal.X * 90.f;
			const float Roll = FMath::FRandRange(-180.0f, 180.0f);
			FRotator DecalRotation(-Pitch, Yaw, Roll);

			UGameplayStatics::SpawnDecalAttached(
				Decal.DecalMaterial
				, FVector(1.f, Decal.DecalSize * DecalScale, Decal.DecalSize * DecalScale)
				, SurfaceHit.Component.Get()
				, SurfaceHit.BoneName
				, SurfaceHit.ImpactPoint
				, DecalRotation
				, EAttachLocation::KeepWorldPosition
				, Decal.LifeSpan
			);
		}
	}

}

void AEXExplosionEffect::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float TimeAlive = GetWorld()->GetTimeSeconds() - CreationTime;
	const float TimeRemaining = FMath::Max(0.0f, ExplosionLightFadeOut - TimeAlive);

	if (TimeRemaining > 0.f)
	{
		const float FadeAlpha = 1.0f - FMath::Square(TimeRemaining / ExplosionLightFadeOut);
		ExplosionLight->SetIntensity(LightIntensity * FadeAlpha);
	}
	else
	{
		Destroy();
	}
}
