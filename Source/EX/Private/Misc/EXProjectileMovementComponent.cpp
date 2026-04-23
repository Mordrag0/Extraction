// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXProjectileMovementComponent.h"
#include "EX.h"
#include "GameFramework/MovementComponent.h"
#include "EXNetDefines.h"


void UEXProjectileMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetOwnerRole() == ROLE_Authority)
	{
		if (bInheritVelocity)
		{
			AActor* Projectile = GetOwner();
			AActor* ProjectileOwner = Projectile ? Projectile->GetOwner() : nullptr;
			if (ProjectileOwner)
			{
				const FVector AddedVelocity = ProjectileOwner->GetVelocity();
				UE_LOG(LogEXWeapon, Log, TEXT("Added velocity = %f, %f, %f,"), AddedVelocity.X, AddedVelocity.Y, AddedVelocity.Z);
				Velocity += ProjectileOwner->GetVelocity();
			}
		}
	}
	OnProjectileStop.AddDynamic(this, &UEXProjectileMovementComponent::Stopped);
}

void UEXProjectileMovementComponent::SetSticky(bool bInSticky)
{
	bSticky = bInSticky;
}

void UEXProjectileMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	const FRotator Rot = UpdatedComponent->GetComponentRotation();
	UpdatedComponent->SetWorldRotation(FRotator(0, Rot.Yaw, 0));
	return;
	if (GetOwnerRole() < ROLE_Authority)
	{
		bSimulationEnabled = false;
		MaxClientOffsetSquared = FMath::Square(MaxClientOffset);
	}
}

void UEXProjectileMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice /*= 0.f*/, const FVector& MoveDelta /*= FVector::ZeroVector*/)
{
	if (bSticky)
	{
		StopSimulating(Hit);
		if (GetOwnerRole() == ENetRole::ROLE_Authority)
		{
			if (Hit.GetActor())
			{
				AActor* Projectile = GetOwner();
				Projectile->AttachToActor(Hit.GetActor(), FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
				GetOwner()->SetReplicateMovement(false);
				StuckHit = Hit;
				MARK_PROPERTY_DIRTY_FROM_NAME(UEXProjectileMovementComponent, StuckHit, this);
			}
		}
	}
	else
	{
		if (Hit.ImpactNormal.Z > BounceCutOff)
		{
			StopSimulating(Hit);
		}
		else
		{
			Super::HandleImpact(Hit, TimeSlice, MoveDelta);
		}
	}
}

void UEXProjectileMovementComponent::Stopped(const FHitResult& ImpactResult)
{
	bStopped = true;
}


void UEXProjectileMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEXProjectileMovementComponent::MoveInterpolationTarget(const FVector& NewLocation, const FRotator& NewRotation)
{
	Super::MoveInterpolationTarget(NewLocation, NewRotation);
}

void UEXProjectileMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(UEXProjectileMovementComponent, StuckHit, PushReplicationParams::Default);
}

void UEXProjectileMovementComponent::TickInterpolation(float DeltaTime)
{
	Super::TickInterpolation(DeltaTime);
}

void UEXProjectileMovementComponent::OnRep_StuckHit()
{
	if (!StuckHit.bBlockingHit)
	{
		return;
	}
	StopSimulating(StuckHit);
	AActor* Projectile = GetOwner();
	Projectile->AttachToActor(StuckHit.GetActor(), FAttachmentTransformRules::KeepWorldTransform, StuckHit.BoneName);
}

