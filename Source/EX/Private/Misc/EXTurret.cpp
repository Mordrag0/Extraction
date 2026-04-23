// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXTurret.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Online/EXPlayerState.h"
#include "Player/EXCharacter.h"
#include "Online/EXTeam.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"
#include "System/EXGameplayStatics.h"

AEXTurret::AEXTurret()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UPoseableMeshComponent>("TurretMesh");
	Mesh->SetupAttachment(Root);

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>("CollisionComp");
	CollisionComp->SetupAttachment(Root);

	EnemySearchArea = CreateDefaultSubobject<USphereComponent>("EnemySearchArea");
	EnemySearchArea->SetupAttachment(Root);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
}

void AEXTurret::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		if (!Target)
		{
			TSet<AActor*> OverlappingActors;
			EnemySearchArea->GetOverlappingActors(OverlappingActors, AEXCharacter::StaticClass());
			for (AActor* Actor : OverlappingActors)
			{
				if (FVector::DistSquared(Actor->GetActorLocation(), GetActorLocation()) < SpottingDistanceSq)
				{
					AEXCharacter* Player = Cast<AEXCharacter>(Actor);
					AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
					if (!PS->IsTeam(Team->GetType()))
					{
						FHitResult Hit;
						if (CanSeeTarget(Player, Hit))
						{
							TargetAquired(Player);
						}
					}
				}
			}
		}
	}
	else if (Target)
	{
		const FVector Direction = (Target->GetActorLocation() - GetActorLocation());
		const FRotator Rot = Direction.ToOrientationRotator();
		const FRotator CurrentRot = Mesh->GetBoneRotationByName("Barrel", EBoneSpaces::WorldSpace);
		Mesh->SetBoneRotationByName("Barrel", FRotator(CurrentRot.Pitch, Rot.Yaw, CurrentRot.Roll), EBoneSpaces::WorldSpace);
	}
}

void AEXTurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXTurret, Target);
}

void AEXTurret::BeginPlay()
{
	Super::BeginPlay();

	SpottingDistanceSq = FMath::Square(SpottingDistance);
	ForgettingDistanceSq = FMath::Square(ForgettingDistance);
}
void AEXTurret::TargetAquired(AEXCharacter* Player)
{
	Target = Player;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &AEXTurret::Fire, FireRate, true, Delay);
}

void AEXTurret::TargetLost()
{
	Target = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
}

bool AEXTurret::CanSeeTarget(AEXCharacter* InTarget, FHitResult& OutHit)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	const bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, GetActorLocation() + FVector::UpVector * BarrelHeight
													 , InTarget->GetActorLocation() + FVector::UpVector * AimHeight, ECC_Visibility_Simple, QueryParams);

	return bHit && (OutHit.GetActor() == InTarget) && (!InTarget->IsDead() || bGib);
}

void AEXTurret::Fire()
{
	FHitResult Hit;
	if (CanSeeTarget(Target, Hit) && (FVector::DistSquared(Hit.TraceStart, Hit.TraceEnd) < ForgettingDistanceSq))
	{
		FPointDamageEvent PointDamageEvent = FPointDamageEvent(Damage, Hit, (Hit.TraceEnd - Hit.TraceStart).GetSafeNormal(), DamageType);
		Hit.GetActor()->TakeDamage(Damage, PointDamageEvent, Target->GetController(), Target);
		Multicast_PlayFireEffects(Hit.TraceEnd);
	}
	else
	{
		TargetLost();
	}
}

void AEXTurret::Multicast_PlayFireEffects_Implementation(const FVector HitLocation)
{
	if (!HasAuthority())
	{
		const FTransform MuzzleTransform = Mesh->GetSocketTransform("MuzzleSocket");
		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleEffect, MuzzleTransform.GetLocation(), MuzzleTransform.Rotator());
		}
		if (TraceEffect)
		{
			UParticleSystemComponent* TraceParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleTransform.GetLocation(), MuzzleTransform.Rotator());
			if (TraceParticle)
			{
				TraceParticle->SetVectorParameter("BeamEnd", HitLocation);
			}
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, MuzzleTransform.GetLocation(), UEXGameplayStatics::GetMasterVolume());
		}
	}
}
