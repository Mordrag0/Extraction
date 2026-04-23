// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXRocket.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/EXCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Inventory/EXHomingMovementComponent.h"
#include "System/EXGameplayStatics.h"
#include "Misc/EXExplosionEffect.h"

AEXRocket::AEXRocket()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	ExplosionPoint = CreateDefaultSubobject<USceneComponent>("ExplosionPoint");
	ExplosionPoint->SetupAttachment(Root);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Rocket");
	Mesh->SetupAttachment(Root);

	Collision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	Collision->SetupAttachment(Mesh);

	Movement = CreateDefaultSubobject<UEXHomingMovementComponent>("Movement2");

	bReplicates = true;
	SetReplicateMovement(true);

	bCanBePickedUp = false;
}

void AEXRocket::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AEXRocket::OnOverlap);
	}
}

void AEXRocket::Explode(bool bComponentHit, const FHitResult Hit)
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;
	TArray<AActor*> IgnoredActors;
	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), DamageRadius, DamageType, IgnoredActors, GetInstigator(), GetInstigatorController());
	Multicast_Reliable_PlayEffects(bComponentHit, Hit);
	OnExploded.Broadcast(this);
	Destroy();
}

void AEXRocket::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Explode(bFromSweep, SweepResult);
}

void AEXRocket::Multicast_Reliable_PlayEffects_Implementation(bool bComponentHit, const FHitResult Hit)
{
	if (!HasAuthority() && ExplosionClass)
	{
		FTransform ExplosionTransform = FTransform(GetActorRotation(), ExplosionPoint->GetComponentLocation());
		UEXGameplayStatics::SpawnExplosion(GetWorld(), ExplosionClass, ExplosionTransform, bComponentHit, Hit);
	}
}

float AEXRocket::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Explode(false, FHitResult());

	return ActualDamage;
}

void AEXRocket::SetTarget(const FVector& Target)
{
	Movement->SetTarget(Target);
}

