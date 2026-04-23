// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/EXAbility.h"
#include "EXGrenade.generated.h"

class UDamageType;

/**
 * 
 */
UCLASS()
class EX_API AEXGrenade : public AEXAbility
{
	GENERATED_BODY()
	
public:
	AEXGrenade();

	FORCEINLINE float GetLifeTime() const { return LifeTime; }

	virtual UEXProjectileMovementComponent* GetProjectileMovementComponent() const override { return ProjectileMovement; }

protected:
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void BeginPlay() override;

	UFUNCTION()
	void Fuse();
	UFUNCTION()
	virtual void Explode();

	// #EXTODORELIABLE
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayExplosionEffects(const FVector& Loc);

	// How much time before direct hits cause an explosion
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float FuseTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float DamageRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float Damage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float GrenadeLifeTime = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	TSubclassOf<class AEXExplosionEffect> ExplosionClass = nullptr;

	FTimerHandle TimerHandle_Explode;
	FTimerHandle TimerHandle_Fuse;

protected:
	bool bDrawDecals = true;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	bool bExplodeOnContact = true;
	bool bFuse = false;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleCollision = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* GrenadeMesh = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ExplosionPoint = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	class UEXProjectileMovementComponent* ProjectileMovement = nullptr;
};
