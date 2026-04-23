// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXFlag.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/EXCharacter.h"
#include "Components/SphereComponent.h"
#include "HUD/EXProgressWidget.h"
#include "System/EXProgress.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"
#include "Components/WidgetComponent.h"
#include "Player/EXPlayerController.h"
#include "Online/EXObjectiveMessage.h"
#include "Online/EXGameModeSW.h"

AEXFlag::AEXFlag()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootScene;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("C4 Mesh"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CaptureWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("CaptureBar"));
	CaptureWidgetComp->SetupAttachment(MeshComp);
	CaptureWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	CaptureComp = CreateDefaultSubobject<UEXProgress>(TEXT("CaptureComp"));
	CaptureComp->SetResetOnStop(false);

	CaptureArea = CreateDefaultSubobject<USphereComponent>(TEXT("Capture Area"));
	CaptureArea->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AEXFlag::Reset()
{
	Super::Reset();
	Attackers.Empty();
	Defenders.Empty();
	CapturingTeam = DefaultCapturingTeam;
	OldProgress = 0.f;
}

void AEXFlag::OnActivated()
{
	Super::OnActivated();
}

bool AEXFlag::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	return false;
}

void AEXFlag::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CaptureComp->OnCompleted.AddUObject(this, &AEXFlag::Captured);

		CaptureArea->OnComponentBeginOverlap.AddDynamic(this, &AEXFlag::PlayerEntered);
		CaptureArea->OnComponentEndOverlap.AddDynamic(this, &AEXFlag::PlayerExited);
	}
	else
	{
		CaptureWidget = Cast<UEXProgressWidget>(CaptureWidgetComp->GetUserWidgetObject());
		if (CaptureWidget)
		{
			CaptureWidget->SetProgressComponent(CaptureComp);
			CaptureWidget->SetColor(CaptureColor);
		}
	}
}

void AEXFlag::Captured(AEXPlayerController* Player)
{
	bool bRecaptured = (CapturingTeam == ETeam::Defense); // If the capturing team are the defenders then the flag was just recaptured
	
	CapturingTeam = bRecaptured ? ETeam::Attack : ETeam::Defense;
	CaptureComp->ResetProgress();
	if (bRecaptured)
	{
		GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 13);
	}
	else
	{
		GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 8);
	}
	GM->FlagCaptured(this, bRecaptured);
}

void AEXFlag::PlayerEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AwardExp();
	AEXCharacter* Player = Cast<AEXCharacter>(OtherActor);
	const AEXPlayerState* const PS = Player ? Player->GetPlayerState<AEXPlayerState>() : nullptr;
	const AEXTeam* const Team = PS ? PS->GetTeam() : nullptr;
	if (Team)
	{
		if (Team->IsAttacking())
		{
			Attackers.Add(Player);
		}
		else
		{
			Defenders.Add(Player);
		}
	}
	RecalculateProgress();
}

void AEXFlag::PlayerExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AwardExp();
	AEXCharacter* Player = Cast<AEXCharacter>(OtherActor);
	const AEXPlayerState* const PS = Player ? Player->GetPlayerState<AEXPlayerState>() : nullptr;
	const AEXTeam* const Team = PS ? PS->GetTeam() : nullptr;
	if (Team)
	{
		if (Team->IsAttacking())
		{
			Attackers.Remove(Player);
		}
		else
		{
			Defenders.Remove(Player);
		}
	}
	RecalculateProgress();
}

void AEXFlag::RecalculateProgress()
{
	int32 ProgressModifier = Attackers.Num() - Defenders.Num();
	if (CapturingTeam == ETeam::Defense)
	{
		ProgressModifier *= -1;
	}
	if (ProgressModifier == 0)
	{
		CaptureComp->StopProgress(nullptr);
	}
	else
	{
		CaptureComp->StartProgress(nullptr, ProgressModifier);
	}
	SetObjectiveActive(Attackers.Num() + Defenders.Num() > 0);
}

void AEXFlag::AwardExp()
{
	float Progress = CaptureComp->GetProgress();
	float DeltaProgress = FMath::Abs(Progress - OldProgress);
	OldProgress = Progress;

	if (Attackers.Num() != Defenders.Num())
	{
		AwardExpToTeam((Attackers.Num() > Defenders.Num()) ? Attackers : Defenders, DeltaProgress);
	}
}

void AEXFlag::AwardExpToTeam(TArray<AEXCharacter*>& Team, float DeltaProgress)
{
	float ExpPerPlayer = DeltaProgress * TotalCapureExp / Team.Num();
	for (AEXCharacter* Player : Team)
	{
		AEXPlayerState* PS = Player->GetPlayerState<AEXPlayerState>();
		if (PS)
		{
			PS->AddScore(ExpPerPlayer, EScoreType::Objective);
		}
	}
}
