// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXStickyNade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/EXCharacter.h"
#include "Player/EXPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/EXProjectileMovementComponent.h"
#include "System/EXGameplayStatics.h"
#include "Components/CapsuleComponent.h"

AEXStickyNade::AEXStickyNade()
{
	GrenadeLifeTime = 0.f;
	FuseTime = 0.f;
	bExplodeOnContact = false;

	ProjectileMovement->SetSticky(true);
}

void AEXStickyNade::Explode()
{
	if (!IsValid(this))
	{
		return;
	}
	bExploded = true;
	NetDestroy();
	Super::Explode();
}

void AEXStickyNade::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		ProjectileMovement->OnProjectileStop.AddDynamic(this, &AEXStickyNade::ProjectileStopped);
	}
}

void AEXStickyNade::OnAttachActorDestroyed(AActor* DestroyedActor)
{
	Destroy();
}

void AEXStickyNade::ProjectileStopped(const FHitResult& ImpactResult)
{
	FCollisionResponseContainer NewResponses;
	NewResponses.SetResponse(ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	NewResponses.SetResponse(ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
	NewResponses.SetResponse(ECC_Pawn, ECollisionResponse::ECR_Ignore);
	CapsuleCollision->SetCollisionResponseToChannels(NewResponses);

	if (ImpactResult.GetActor())
	{
		ImpactResult.GetActor()->OnDestroyed.AddDynamic(this, &AEXStickyNade::OnAttachActorDestroyed);
	}
}

bool AEXStickyNade::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	return ProjectileMovement->IsStopped() && bLookAt && CanPickUp(Player);
}

float AEXStickyNade::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (!bExploded)
	{
		Explode();
	}
	return ActualDamage;
}

