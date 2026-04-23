// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/EXPlantSite.h"
#include "Player/EXCharacter.h"
#include "Misc/EXC4.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Online/EXPlayerState.h"
#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXInventoryInteract.h"
#include "EX.h"
#include "Misc/EXGenerator.h"
#include "System/EXProgress.h"
#include "Misc/EXObjectiveTool.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Online/EXGameModeSW.h"
#include "Online/EXObjectiveMessage.h"
#include "Components/WidgetComponent.h" 
#include "Net/UnrealNetwork.h"
#include "EXNetDefines.h"


AEXPlantSite::AEXPlantSite()
{
	RootScene = CreateDefaultSubobject<USceneComponent>("RootScene");
	RootComponent = RootScene;
	Area = CreateDefaultSubobject<UBoxComponent>("Area");
	Area->SetupAttachment(RootScene);

	IconWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Icon"));
	IconWidgetComp->SetupAttachment(RootScene);
	IconWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	bReplicates = true; // For PlantedC4

	bOneInteractAtOnce = false;

	C4ProgressComp = CreateDefaultSubobject<UEXProgress>(TEXT("C4Progress"));
}

bool AEXPlantSite::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Super::CanInteract_Implementation(Player, bLookAt))
	{
		return false;
	}
	if (!CanBePlanted())
	{
		return false;
	}
	AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
	if (!PS || !PS->IsTeam(PlantingTeam))
	{
		return false;
	}
	if (!Player->GetMovementComponent()->IsMovingOnGround())
	{
		return false;
	}
	return Area->IsOverlappingActor(Player);
}

bool AEXPlantSite::StartInteract_Implementation(AEXCharacter* Player)
{
	Super::StartInteract_Implementation(Player);

	Player->GetInventoryComponent()->EquipTool(RequiredTool, this);
	if (HasAuthority())
	{
		InteractingPlayers.Add(Player);
	}
	return true;
}

bool AEXPlantSite::StopInteract_Implementation(AEXCharacter* Player)
{
	Super::StopInteract_Implementation(Player);

	Player->GetInventoryComponent()->Unequip(Player->GetInventoryComponent()->GetTool(RequiredTool), true);
	
	if (HasAuthority())
	{
		InteractingPlayers.Remove(Player);
		Player->StopProgress();
		Player->OnInteractCompleted.RemoveDynamic(this, &AEXPlantSite::OnPlanted);
	}
	return true;
}

void AEXPlantSite::Reset()
{
	Super::Reset();

	if (PlantedC4)
	{
		PlantedC4->Destroy();
		PlantedC4 = nullptr;
		MARK_PROPERTY_DIRTY_FROM_NAME(AEXPlantSite, PlantedC4, this);
	}
}

void AEXPlantSite::ChangeIconVisibility(bool bVisible)
{
	// Only show sites that can be planted, but allow hiding all so it can't get stuck on visible
	if (!bVisible || CanBePlanted())
	{
		IconWidgetComp->SetVisibility(bVisible, true);
	}
}

bool AEXPlantSite::CanBePlanted() const
{
	bool bCurrentlyPlanted = (PlantedC4 != nullptr) && IsValid(PlantedC4);
	if (bCurrentlyPlanted)
	{
		return false;
	}
	if (Generator && !Generator->IsRepaired())
	{
		return false;
	}
	return (GetState() == EInteractableState::Active);
}

void AEXPlantSite::SetGenerator(AEXGenerator* InGenerator)
{
	Generator = InGenerator;
	PlantingTeam = Generator->GetRepairingTeam() == ETeam::Attack ? ETeam::Defense : ETeam::Attack;
}

void AEXPlantSite::StartProgress(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		// This interact is different than others, because we can have multiple players planting independently
		// So we use the Progress component of the player to display progress and bind OnPlanted

		const float ProgressModifier = Cast<UEXInventoryInteract>(Player->GetInventoryComponent()->GetTool(RequiredTool))->GetProgressModifier();
		Player->StartProgress(TimeToPlant, ProgressModifier);
		Player->OnInteractCompleted.AddDynamic(this, &AEXPlantSite::OnPlanted);
	}
}

void AEXPlantSite::OnPlanted(AEXCharacter* Player)
{
	{
		const int32 NumInteracting = InteractingPlayers.Num();
		for (int32 PlayerIdx = NumInteracting - 1; PlayerIdx >= 0; --PlayerIdx)
		{
			InteractingPlayers[PlayerIdx]->StopProgress();
			InteractingPlayers[PlayerIdx]->StopInteract(this, true); // This will edit the InteractingPlayers array
		}
	}

	SetObjectiveActive(true);
	if (C4Class)
	{
		const FVector Loc = Player->GetActorLocation();
		FHitResult Hit;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Player);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Loc, Loc + FVector(0, 0, -300), ECC_WorldOnly, CollisionParams))
		{
			FRotator Rot = Player->GetActorRotation().Add(0, 180, 0); // Rotate towards the player who planted
			FActorSpawnParameters SpawnParams;
			SpawnParams.Instigator = Player;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			PlantedC4 = GetWorld()->SpawnActor<AEXC4>(C4Class, Hit.ImpactPoint, Rot, SpawnParams);
			if (PlantedC4)
			{
				PlantedC4->SetProgressComp(C4ProgressComp);
				PlantedC4->StartTimer(TimeToExplode);
				PlantedC4->GetDefuseComp()->OnCompleted.AddUObject(this, &AEXPlantSite::OnDefused);
				if (Generator)
				{
					PlantedC4->SetTarget(Generator);
				}
				MARK_PROPERTY_DIRTY_FROM_NAME(AEXPlantSite, PlantedC4, this);
			}
		}
	}
	
	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 5, Player->GetPlayerState<APlayerState>());
}

void AEXPlantSite::OnDefused(AEXPlayerController* Player)
{
	ClearC4();
}

void AEXPlantSite::OnCompleted(AEXPlayerController* Player)
{
	ClearC4();

	Super::OnCompleted(Player);
}

void AEXPlantSite::OnRep_PlantedC4()
{
	if (PlantedC4)
	{
		PlantedC4->SetProgressComp(C4ProgressComp);
	}
}

void AEXPlantSite::ClearC4()
{
	SetObjectiveActive(false);
	PlantedC4 = nullptr;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXPlantSite, PlantedC4, this);
}

void AEXPlantSite::BeginPlay()
{
	Super::BeginPlay();

	IconWidgetComp->SetVisibility(false, true);
	SetProgressComp(C4ProgressComp);
}

void AEXPlantSite::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(AEXPlantSite, PlantedC4, PushReplicationParams::Default);
}
