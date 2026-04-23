// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXAbility.h"
#include "Player/EXCharacter.h"
#include "GameFramework/Controller.h"
#include "Inventory/EXInventoryComponent.h"
#include "Player/EXPlayerController.h"
#include "Online/EXLevelRules.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/EXInventory.h"
#include "System/EXGameplayStatics.h"
#include "Misc/EXProjectileMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "EXNetDefines.h"

AEXAbility::AEXAbility()
{
}

void AEXAbility::Init(UEXInventory* InventoryOwner)
{
	Inventory = InventoryOwner;

	if (!bNeedsEyeContactToInteract)
	{
		AEXLevelRules* LR = UEXGameplayStatics::GetLevelRules(this);
		LR->AddInteractable(this);
		OnDestroyed.AddDynamic(LR, &AEXLevelRules::RemoveInteractable);
	}
}

void AEXAbility::PickUp(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		Inventory->RecoverAbility(1.f);
		Destroy();
	}
}

bool AEXAbility::CanPickUp(const AEXCharacter* Player) const
{ 
	return (bCanBePickedUp || (bCanBeRecycled && Player->CanRecycle())) 
		&& (GetInstigatorController() == Player->GetEXController());
}

void AEXAbility::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && (LifeTime > 0))
	{
		SetLifeSpan(LifeTime);
	}
}

void AEXAbility::NetDestroy()
{
	SetLifeSpan(5.f);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bDestroyed = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXAbility, bDestroyed, this);
	if (!IsNetMode(NM_DedicatedServer))
	{
		PlayDeathEffects();
	}
}

void AEXAbility::OnRep_Destroyed()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	PlayDeathEffects();
}

void AEXAbility::PlayDeathEffects()
{
	if (DeathEffect)
	{
		UParticleSystemComponent* DeathParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEffect, GetActorLocation(), GetActorRotation());
	}
	if (DeathSoundEffect)
	{
		const float VolumeMultiplier = UEXGameplayStatics::GetMasterVolume();
		UAudioComponent* DeathAudio = UGameplayStatics::SpawnSoundAtLocation(this, DeathSoundEffect, GetActorLocation(), GetActorRotation(), VolumeMultiplier, 1.f, 0.f);
	}
}

bool AEXAbility::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	return bLookAt && CanPickUp(Player);
}

bool AEXAbility::StartInteract_Implementation(AEXCharacter* Player)
{
	return true;
}

bool AEXAbility::StopInteract_Implementation(AEXCharacter* Player)
{
	PickUp(Player);
	return true;
}

bool AEXAbility::TapInteract() const
{
	return true;
}

void AEXAbility::PostNetReceiveLocationAndRotation()
{
	const FRepMovement& ConstRepMovement = GetReplicatedMovement();

	// Always consider Location as changed if we were spawned this tick as in that case our replicated Location was set as part of spawning, before PreNetReceive()
	if ((FRepMovement::RebaseOntoLocalOrigin(ConstRepMovement.Location, this) == GetActorLocation()
		 && ConstRepMovement.Rotation == GetActorRotation()) && (CreationTime != GetWorld()->TimeSeconds))
	{
		return;
	}

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Correction to make sure pawn doesn't penetrate floor after replication rounding
		const FRepMovement& RepMovement = GetReplicatedMovement();

		UEXProjectileMovementComponent* ProjectileMovement = GetProjectileMovementComponent();
		if (ProjectileMovement)
		{
			ProjectileMovement->MoveInterpolationTarget(RepMovement.Location, RepMovement.Rotation);
		}
		else
		{
			Super::PostNetReceiveLocationAndRotation();
		}
	}
}

void AEXAbility::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	UEXProjectileMovementComponent* ProjectileMovement = GetProjectileMovementComponent();
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = NewVelocity;
	}
}

void AEXAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(AEXAbility, bDestroyed, PushReplicationParams::Default);
}
