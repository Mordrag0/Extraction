// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXEV.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Player/EXCharacter.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"
#include "System/EXProgress.h"
#include "HUD/EXProgressWidget.h"
#include "Engine/SplineMeshActor.h"
#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXInventoryInteract.h"
#include "Misc/EXBarricade.h"
#include "Online/EXLevelRules.h"
#include "Online/EXGameModeSW.h"
#include "Misc/EXEVMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/EXPlayerController.h"
#include "Online/EXObjectiveMessage.h"
#include "System/EXGameplayStatics.h"
#include "Misc/EXEVPath.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "EXEV"

AEXEV::AEXEV()
{
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = Mesh;
	Area = CreateDefaultSubobject<USphereComponent>("Area");
	Area->SetupAttachment(Mesh);
	Area->OnComponentBeginOverlap.AddDynamic(this, &AEXEV::PlayerEnteredArea);
	Area->OnComponentEndOverlap.AddDynamic(this, &AEXEV::PlayerLeftArea);


	RepairWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("RepairProgressbar"));
	RepairWidgetComp->SetupAttachment(Mesh);
	RepairWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	DestroyWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DestroyProgressbar"));
	DestroyWidgetComp->SetupAttachment(Mesh);
	DestroyWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	EVProgressComp = CreateDefaultSubobject<UEXProgress>("ProgressComp");
	RepairComp = CreateDefaultSubobject<UEXProgress>("RepairComp");
	DestroyComp = CreateDefaultSubobject<UEXProgress>("DestroyComp");

	Movement = CreateDefaultSubobject<UEXEVMovementComponent>("Movement");

	bDamagable = true;

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(false);

}

void AEXEV::BeginPlay()
{
	Super::BeginPlay();
	
	LevelRules = UEXGameplayStatics::GetLevelRules(this);

	RepairComp->SetResetOnStop(false);
	RepairComp->SetGoal(MaxHealth / RepairSpeed);
	DestroyComp->SetResetOnStop(false);
	DestroyComp->SetGoal(MaxHealth);

	Barricades.Sort([](const AEXBarricade& A, const AEXBarricade& B)
					{
						return A.GetBlockingDistance() < B.GetBlockingDistance();
					});

	Movement->Init();

	if (HasAuthority())
	{
		RepairComp->OnCompleted.AddUObject(this, &AEXEV::OnRepaired);
		DestroyComp->OnCompleted.AddUObject(this, &AEXEV::OnDestroyed);
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
	SetProgressComp(EVProgressComp);

	Health = MaxHealth;
}

float AEXEV::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!bRepaired)
	{
		return 0;
	}

	Health -= ActualDamage;
	DestroyComp->AddProgressInstant(Cast<AEXPlayerController>(EventInstigator), ActualDamage);
	return ActualDamage;
}

USplineComponent* AEXEV::GetPath() const
{
	return CastChecked<USplineComponent>(PathActor->GetComponentByClass(USplineComponent::StaticClass()));
}

void AEXEV::PlayerEnteredArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEXCharacter* Player = Cast<AEXCharacter>(OtherActor);
	AEXPlayerState* PS = Player ? Player->GetPlayerState<AEXPlayerState>() : nullptr;
	AEXTeam* Team = PS ? PS->GetTeam() : nullptr;

 	if (Team && Team->IsAttacking())
	{
		MovingPlayer = Player;
	}
}

void AEXEV::PlayerLeftArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (MovingPlayer == Cast<AEXCharacter>(OtherActor))
	{
		MovingPlayer = nullptr;

		// When a player stops moving the EV, check if another can take over
		TSet<AActor*> Players;
		Area->GetOverlappingActors(Players, AEXCharacter::StaticClass());
		for (AActor* Actor : Players)
		{
			AEXCharacter* Player = Cast<AEXCharacter>(Actor);
			AEXPlayerState* PS = Player ? Player->GetPlayerState<AEXPlayerState>() : nullptr;
			AEXTeam* Team = PS ? PS->GetTeam() : nullptr;
			if (Team && Team->IsAttacking())
			{
				MovingPlayer = Player;
				break;
			}
		}
	}
}

void AEXEV::OnRepaired(AEXPlayerController* Player)
{
	bRepaired = true;
	Health = MaxHealth;
	DestroyComp->ResetProgress();
	Player->GetEXCharacter()->StopInteract(this, true);
	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 9, Player->GetPlayerState<APlayerState>());
	SetCanBeDamaged(true);
}

void AEXEV::OnDestroyed(AEXPlayerController* Player)
{
	bRepaired = false;
	RepairComp->ResetProgress();
	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 10, Player->GetPlayerState<APlayerState>());
	SetCanBeDamaged(false);
}

void AEXEV::OnCompleted(AEXPlayerController* Player)
{
	SetObjectiveActive(false);

	Super::OnCompleted(Player);
	bFinished = true;
	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 11, Player->GetPlayerState<APlayerState>());
}

void AEXEV::Passed()
{
	Super::Passed();
	SetCanBeDamaged(false);
}

void AEXEV::SetActive()
{
	Super::SetActive();

	if (bRepaired)
	{
		SetCanBeDamaged(true);
	}
}

void AEXEV::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXEV, bRepaired);
}

bool AEXEV::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Super::CanInteract_Implementation(Player, bLookAt))
	{
		return false;
	}
	if (!bRepaired && bLookAt)
	{
		AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
		AEXTeam* Team = PS ? PS->GetTeam() : nullptr;
		if (Team && Team->IsAttacking())
		{
			return true;
		}
	}
	return false;
}

bool AEXEV::StartInteract_Implementation(AEXCharacter* Player)
{
	Super::StartInteract_Implementation(Player);

	Player->GetInventoryComponent()->EquipTool(RequiredTool, this);
	return true;
}

bool AEXEV::StopInteract_Implementation(AEXCharacter* Player)
{
	Super::StopInteract_Implementation(Player);

	Player->GetInventoryComponent()->Unequip(Player->GetInventoryComponent()->GetTool(RequiredTool), true);
	if (HasAuthority())
	{
		RepairComp->StopProgress(Player->GetEXController());
	}
	return true;
}

void AEXEV::Reset()
{
	Super::Reset();

	bRepaired = false;
	bFinished = false;
	EVProgressComp->ResetProgress();
	RepairComp->ResetProgress();
	DestroyComp->ResetProgress();
	Health = MaxHealth;

	CurrentBarricadeIdx = 0;
	CurrentSpawnCheckIdx = 0;
	for (AEXBarricade* Barricade : Barricades)
	{
		Barricade->Reset(); // Only necessary if we're manually calling this function and not reset on everything
	}
	Movement->Reset();
}

void AEXEV::StartProgress(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		// We already checked that the new player can repair faster than the current player in CanInteract()
		if (RepairingPlayer)
		{
			Player->StopInteract(this, true);
			RepairComp->StopProgress(Player->GetEXController());
		}
		const float ProgressModifier = Cast<IEXObjectiveTool>(Player->GetInventoryComponent()->GetTool(RequiredTool))->GetProgressModifier();
		RepairComp->StartProgress(Player->GetEXController(), ProgressModifier);
	}
}

#undef LOCTEXT_NAMESPACE
