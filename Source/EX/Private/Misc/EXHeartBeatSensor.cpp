// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXHeartBeatSensor.h"
#include "Misc/EXProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Player/EXCharacter.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"

AEXHeartBeatSensor::AEXHeartBeatSensor()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(Mesh);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);

	Movement = CreateDefaultSubobject<UEXProjectileMovementComponent>("Movement");
	Movement->OnProjectileStop.AddDynamic(this, &AEXHeartBeatSensor::OnMovementStopped);

	bReplicates = true;
	SetReplicateMovement(true);

	SetCanBeDamaged(true);
}

void AEXHeartBeatSensor::BeginPlay()
{
	Super::BeginPlay();

	EXCharacter = Cast<AEXCharacter>(GetInstigator());
}

float AEXHeartBeatSensor::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Destroy();

	return ActualDamage;
}

void AEXHeartBeatSensor::OnMovementStopped(const FHitResult& ImpactResult)
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Spotting, this, &AEXHeartBeatSensor::SpotNearbyEnemies, TimeBetweenSpots, true, InitialDelay);
	}
}

void AEXHeartBeatSensor::SpotNearbyEnemies()
{
	--TimeBetweenSpots;
	if (TimeBetweenSpots == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Spotting);
		SetLifeSpan(SpottingDuration);
	}

	TSet<AActor*> OverlappingPlayers;
	Collision->GetOverlappingActors(OverlappingPlayers, AEXCharacter::StaticClass());

	ETeam Team = EXCharacter->GetPlayerState<AEXPlayerState>()->GetTeam()->GetType();
	for (AActor* Actor : OverlappingPlayers)
	{
		AEXCharacter* Player = Cast<AEXCharacter>(Actor);
		if (!Player->GetPlayerState<AEXPlayerState>()->IsTeam(Team))
		{
			Player->Spotted(SpottingDuration);
		}
	}
}

