// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXHealingStation.h"
#include "Components/StaticMeshComponent.h"
#include "Player/EXCharacter.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"

AEXHealingStation::AEXHealingStation()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	CollisionComp->SetupAttachment(Root);

}

void AEXHealingStation::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEXHealingStation::OnPlayerBeginOverlap);
	}
}

void AEXHealingStation::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEXCharacter* Player = Cast<AEXCharacter>(OtherActor);
	if (Player)
	{
		AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
		if (PS->IsTeam(Team->GetType()))
		{
			Player->SetHealingRate(HealingRate, GetInstigatorController());
		}
	}
}
