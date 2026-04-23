// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXLaser.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/MovementComponent.h"
#include "EX.h"
#include "Misc/EXLaserMovementComponent.h"

AEXLaser::AEXLaser()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Laser = CreateDefaultSubobject<UStaticMeshComponent>("Laser");
	Laser->SetupAttachment(Root);

	Collision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	Collision->SetupAttachment(Root);

	Movement = CreateDefaultSubobject<UEXLaserMovementComponent>("Movement");


	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	SetReplicateMovement(true);
}

void AEXLaser::BeginPlay()
{
	Super::BeginPlay();
	
	Movement->SetTargetLocation(GetActorLocation());
	SetLifeSpan(LifeSpan);

	if (HasAuthority())
	{
		Collision->SetCollisionResponseToAllChannels(ECR_Overlap);
	}
}

void AEXLaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> OverlappedActors;
	Collision->GetOverlappingActors(OverlappedActors);
	for (AActor* Actor : OverlappedActors)
	{
		if (Actor->CanBeDamaged())
		{
			Actor->TakeDamage(DPS * DeltaTime, DamageEvent, GetInstigatorController(), this);
		}
	}
}

void AEXLaser::SetTargetLocation(const FVector InTargetLocation)
{
	Movement->SetTargetLocation(InTargetLocation);
}
