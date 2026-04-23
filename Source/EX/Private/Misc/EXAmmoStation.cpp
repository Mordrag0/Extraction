// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXAmmoStation.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventoryComponent.h"
#include "Online/EXPlayerState.h"
#include "Online/EXGameModeBase.h"

AEXAmmoStation::AEXAmmoStation()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	CollisionComp->SetupAttachment(Root);
}

void AEXAmmoStation::GiveAmmo()
{
	if (HasAuthority())
	{
		TSet<AActor*> OverlappingPlayers;
		CollisionComp->GetOverlappingActors(OverlappingPlayers, AEXCharacter::StaticClass());
		for (AActor* Actor : OverlappingPlayers)
		{
			AEXCharacter* Player = Cast<AEXCharacter>(Actor);
			UEXInventoryComponent* InventoryComp = Player->GetInventoryComponent();
			const int32 MagsGiven = InventoryComp->AddAmmo(MagAmount);

			AEXPlayerState* PS = GetInstigatorController()->GetPlayerState<AEXPlayerState>();
			AEXGameModeBase* GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
			if (PS && GM)
			{
				PS->AddScore(MagsGiven * GM->GetScorePerMagGiven(), EScoreType::Support);
			}
		}
	}
}

void AEXAmmoStation::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_GiveAmmo, this, &AEXAmmoStation::GiveAmmo, Frequency, true);
	}
}
