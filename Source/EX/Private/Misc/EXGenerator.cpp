// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXGenerator.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/EXCharacter.h"
#include "System/EXProgress.h"
#include "Components/WidgetComponent.h"
#include "HUD/EXProgressWidget.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"
#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXInventoryInteract.h"
#include "Player/EXPlayerController.h"
#include "Online/EXGameModeSW.h"
#include "Online/EXObjectiveMessage.h"
#include "GameFramework/PlayerState.h"
#include "Misc/EXC4.h"
#include "Misc/EXPlantSite.h"
#include "Net/UnrealNetwork.h"
#include "EXNetDefines.h"


AEXGenerator::AEXGenerator()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	RepairWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("RepairProgressbar"));
	RepairWidgetComp->SetupAttachment(Mesh);
	RepairWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	DestroyWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DestroyProgressbar"));
	DestroyWidgetComp->SetupAttachment(Mesh);
	DestroyWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);


	RepairComp = CreateDefaultSubobject<UEXProgress>("RepairComp");
	DestroyComp = CreateDefaultSubobject<UEXProgress>("DestroyComp");

	PlantSiteChild = CreateDefaultSubobject<UChildActorComponent>("PlantSiteChild");
	PlantSiteChild->SetupAttachment(Root);

	bDamagable = true;

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void AEXGenerator::BeginPlay()
{
	Super::BeginPlay();

	PlantSite = Cast<AEXPlantSite>(PlantSiteChild->GetChildActor());
	PlantSite->SetGenerator(this);
	
	RepairComp->SetResetOnStop(false);
	DestroyComp->SetResetOnStop(false);
	DestroyComp->SetGoal(MaxHealth);

	Reset();

	if (HasAuthority())
	{
		RepairComp->OnCompleted.AddUObject(this, &AEXGenerator::OnRepaired);
	}
	else
	{
		RepairWidget = Cast<UEXProgressWidget>(RepairWidgetComp->GetUserWidgetObject());
		if (RepairWidget)
		{
			RepairWidget->SetProgressComponent(RepairComp);
			RepairWidget->SetColor(HealthbarColor);
		}

		DestroyWidget = Cast<UEXProgressWidget>(DestroyWidgetComp->GetUserWidgetObject());
		if (DestroyWidget)
		{
			DestroyWidget->SetProgressComponent(DestroyComp, true);
			DestroyWidget->SetColor(HealthbarColor);
		}
	}
}

bool AEXGenerator::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Super::CanInteract_Implementation(Player, bLookAt))
	{
		return false;
	}

	const AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
	const AEXTeam* Team = PS ? PS->GetTeam() : nullptr;
	const bool bOnRepairingTeam = Team && (Team->GetType() == RepairingTeam);

	return !bRepaired && bLookAt && bOnRepairingTeam;
}

bool AEXGenerator::StartInteract_Implementation(AEXCharacter* Player)
{
	Super::StartInteract_Implementation(Player);

	Player->GetInventoryComponent()->EquipTool(RequiredTool, this);
	return true;
}

bool AEXGenerator::StopInteract_Implementation(AEXCharacter* Player)
{
	Super::StopInteract_Implementation(Player);

	Player->GetInventoryComponent()->Unequip(Player->GetInventoryComponent()->GetTool(RequiredTool), true);
	if (HasAuthority())
	{
		RepairComp->StopProgress(Player->GetEXController());
	}
	return true;
}

void AEXGenerator::StartProgress(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		const float ProgressModifier = Cast<IEXObjectiveTool>(Player->GetInventoryComponent()->GetTool(RequiredTool))->GetProgressModifier();
		RepairComp->StartProgress(Player->GetEXController(), ProgressModifier);
	}
}

float AEXGenerator::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!bRepaired || !IsObjectiveActive())
	{
		return 0;
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ActualDamage = FMath::Min(ActualDamage, Health);
	Health -= ActualDamage;
	AEXCharacter* Player = EventInstigator ? Cast<AEXCharacter>(EventInstigator->GetPawn()) : nullptr;
	AEXPlayerController* EXPC = Cast<AEXPlayerController>(EventInstigator);
	if (FMath::IsNearlyZero(Health))
	{
		OnDestroyed(EXPC);
	}
	DestroyComp->AddProgressInstant(EXPC, ActualDamage);
	return ActualDamage;
}

void AEXGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(AEXGenerator, bRepaired, PushReplicationParams::Default);
}

void AEXGenerator::Kill(AEXPlayerController* Player)
{
	DestroyComp->AddProgressInstant(Player, Health);
	Health = 0;
	OnDestroyed(Player);
}

void AEXGenerator::Reset()
{
	Super::Reset();
	RepairComp->ResetProgress();
	DestroyComp->ResetProgress();

	bRepaired = bRepairedByDefault;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXGenerator, bRepaired, this);
	if (bRepairedByDefault)
	{
		Health = MaxHealth;
	}
}

void AEXGenerator::Passed()
{
	Super::Passed();

	if(HasAuthority())
	{
		if (C4)
		{
			C4->Destroy();
		}
	}
	SetCanBeDamaged(false);
}

void AEXGenerator::SetActive()
{
	Super::SetActive();

	if (bRepaired)
	{
		SetCanBeDamaged(true);
	}
}

void AEXGenerator::OnRepaired(AEXPlayerController* Player)
{
	bRepaired = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXGenerator, bRepaired, this);
	SetCanBeDamaged(true);
	Health = MaxHealth;
	DestroyComp->ResetProgress();
	Player->GetEXCharacter()->StopInteract(this, true);
	APlayerState* PS = Player ? Player->GetPlayerState<APlayerState>() : nullptr;
	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 3, PS);

	OnGeneratorRepaired();
}

void AEXGenerator::OnDestroyed(AEXPlayerController* Player)
{
	bRepaired = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXGenerator, bRepaired, this);
	SetCanBeDamaged(false);
	RepairComp->ResetProgress();
	APlayerState* PS = Player ? Player->GetPlayerState<APlayerState>() : nullptr;
	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 4, PS);

	OnGeneratorDestroyed();
}
