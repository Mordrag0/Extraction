// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXGrenade.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXCharacter.h"
#include "Misc/EXEV.h"
#include "Misc/EXProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Misc/EXExplosionEffect.h"
#include "System/EXGameplayStatics.h"
#include "EX.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/DamageType.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// #DEBUG
static TAutoConsoleVariable<int32> CVarShowGrenadeDebug(
	TEXT("x.ShowGrenadeDebug"),
	0,
	TEXT("Enabled rendering debug info for character grenades.\n")
	TEXT("  0: off\n")
	TEXT("  1: on\n"),
	ECVF_Cheat);
#endif

AEXGrenade::AEXGrenade()
{
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetCollisionProfileName("Projectile");
	CapsuleCollision->BodyInstance.SetCollisionProfileName("Projectile");
	CapsuleCollision->bTraceComplexOnMove = true;
	RootComponent = CapsuleCollision;

	GrenadeMesh = CreateDefaultSubobject<USkeletalMeshComponent>("GrenadeMesh");
	GrenadeMesh->SetupAttachment(CapsuleCollision);

	CapsuleCollision->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CapsuleCollision->CanCharacterStepUpOn = ECB_No;

	ExplosionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ExplosionPoint"));
	ExplosionPoint->SetupAttachment(GrenadeMesh);

	ProjectileMovement = CreateDefaultSubobject<UEXProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CapsuleCollision;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	bReplicates = true;
	//bReplicateMovement = true;

	DamageTypeClass = UDamageType::StaticClass();
}

void AEXGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->CanBeDamaged() && !bFuse)
	{
		Explode();
	}
}

void AEXGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		FTimerManager& WorldTimerManager = GetWorld()->GetTimerManager();
		if (!WorldTimerManager.IsTimerActive(TimerHandle_Explode) && (GrenadeLifeTime > 0))
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Explode, this, &AEXGrenade::Explode, GrenadeLifeTime);
		}
		if (FuseTime > 0)
		{
			bFuse = true;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fuse, this, &AEXGrenade::Fuse, FuseTime);
		}
		if (bExplodeOnContact)
		{
			CapsuleCollision->OnComponentHit.AddDynamic(this, &AEXGrenade::OnHit);
		}
	}
	ProjectileMovement->SetInterpolatedComponent(GrenadeMesh);
}

void AEXGrenade::Fuse()
{
	bFuse = false;
}

void AEXGrenade::Explode()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	const FVector Loc = ExplosionPoint->GetComponentLocation();
	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, Loc, DamageRadius, DamageTypeClass, IgnoreActors, this, GetInstigatorController(), false, ECC_Visibility_Simple);

	Multicast_PlayExplosionEffects(Loc);

	Destroy();
}

void AEXGrenade::Multicast_PlayExplosionEffects_Implementation(const FVector& Loc)
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		const AEXExplosionEffect* const ExplosionCDO = ExplosionClass->GetDefaultObject<AEXExplosionEffect>();
		FVector HitLoc = FVector::ZeroVector;
		if (bDrawDecals)
		{
			// When checking collisions in this method there is no need to ignore 'this' because it should be ignoring ECC_WorldOnly
			FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionCDO->MaxImpactDistance);

			TArray<FOverlapResult> Overlaps;
			GetWorld()->OverlapMultiByChannel(Overlaps, Loc, FQuat::Identity, ECC_WorldOnly, Sphere);

			float ClosestDistanceSq = MAX_FLT;
			for (FOverlapResult& Overlap : Overlaps) // If all the actors that are within [ExplosionCDO->MaxImpactDistance] distance, find the closest one
			{
				FVector ClosestPoint;
				if (UEXGameplayStatics::GetClosestPointOnCollision(Overlap.GetActor(), Loc, ECC_WorldOnly, ClosestPoint))
				{
					const float DistSquared = FVector::DistSquared(Loc, ClosestPoint);
					if (DistSquared < ClosestDistanceSq)
					{
						ClosestDistanceSq = DistSquared;
						HitLoc = ClosestPoint;
					}
				}
			}
		}
		bool bComponentHit = false;
		FHitResult Hit;
		if (!HitLoc.IsZero())
		{
			FVector Direction = HitLoc - Loc;
			Direction.Normalize();
			bComponentHit = GetWorld()->LineTraceSingleByChannel(Hit, Loc, Loc + Direction * ExplosionCDO->MaxImpactDistance, ECC_WorldOnly);
		}
		FTransform ExplosionTransform = FTransform(GetActorRotation(), Loc, GetActorScale());
		UEXGameplayStatics::SpawnExplosion(
			GetWorld(), 
			ExplosionClass, 
			FTransform(UKismetMathLibrary::RandomRotator(), Loc, ExplosionCDO->GetActorScale()),
			bComponentHit, 
			Hit);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) 
		const int32 bShowDebug = CVarShowGrenadeDebug.GetValueOnGameThread();
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 32, FColor::Yellow, false, 5.f);
		}
#endif
	}
}
