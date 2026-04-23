// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/EXC4.h"
#include "HUD/EXTimerWidget.h"
#include "Components/WidgetComponent.h"
#include "System/EXProgress.h"
#include "HUD/EXProgressWidget.h"
#include "Player/EXCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Online/EXPlayerState.h"
#include "Inventory/EXInventoryComponent.h"
#include "Inventory/EXInventoryInteract.h"
#include "System/EXGameplayStatics.h"
#include "EX.h"
#include "Inventory/EXDamageType.h"
#include "DrawDebugHelpers.h"
#include "Misc/EXGenerator.h"
#include "Player/EXPlayerController.h"
#include "GameFramework/DamageType.h"
#include "Online/EXObjectiveMessage.h"
#include "Online/EXGameModeBase.h"
#include "Online/EXTeam.h"
#include "Net/UnrealNetwork.h"
#include "EXNetDefines.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
// #DEBUG
static TAutoConsoleVariable<int32> CVarShowC4Debug(
	TEXT("x.ShowC4Debug"),
	0,
	TEXT("Enables rendering a debug sphere for C4.\n")
	TEXT("  0: off\n")
	TEXT("  1: on\n"),
	ECVF_Cheat);
#endif

AEXC4::AEXC4()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootScene;

	DamageSourceLocation = CreateDefaultSubobject<USceneComponent>(TEXT("DamageSourceLocation"));
	DamageSourceLocation->SetupAttachment(RootComponent);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("C4 Mesh"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TimerWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("TimerWidgetComp"));
	TimerWidgetComp->SetupAttachment(MeshComp);
	ProgressWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Progressbar"));
	ProgressWidgetComp->SetupAttachment(MeshComp);
	ProgressWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	DefuseWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Defusebar"));
	DefuseWidgetComp->SetupAttachment(MeshComp);
	DefuseWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	DefuseComp = CreateDefaultSubobject<UEXProgress>(TEXT("DefuseComp"));

	DamageTypeClass = UDamageType::StaticClass();

	bReplicates = true;
}

void AEXC4::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		DefuseComp->OnCompleted.AddUObject(this, &AEXC4::OnDefused);

		AEXPlayerController* EXPC = Cast<AEXPlayerController>(GetInstigatorController());
		PlantingTeam = EXPC ? EXPC->GetPS()->GetTeam()->GetType() : ETeam::Invalid;
		if (PlantingTeam == ETeam::Invalid)
		{
			return;
		}
		MARK_PROPERTY_DIRTY_FROM_NAME(AEXC4, PlantingTeam, this);
	}
	else
	{
		UserWidget = Cast<UEXTimerWidget>(TimerWidgetComp->GetUserWidgetObject());
		ProgressWidget = Cast<UEXProgressWidget>(ProgressWidgetComp->GetUserWidgetObject());
		if (ProgressWidget)
		{
			ProgressWidget->SetColor(ProgressbarColor);
		}
		DefuseWidget = Cast<UEXProgressWidget>(DefuseWidgetComp->GetUserWidgetObject());
		if (DefuseWidget)
		{
			DefuseWidget->SetProgressComponent(DefuseComp);
			DefuseWidget->SetColor(DefusebarColor);
		}
	}
	SetActive();
}

void AEXC4::Explode(AEXPlayerController* Player)
{
	StopInteraction();

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	if (Target)
	{
		AEXGenerator* Gen = Cast<AEXGenerator>(Target);
		if (Gen)
		{
			Gen->Kill(Player);
		}
	}
	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, DamageSourceLocation->GetComponentLocation(), DamageRadius, DamageTypeClass, IgnoreActors, this, Player, false, ECC_Visibility_Simple);
	NetDestroy();
}

void AEXC4::UpdateTimer(float Seconds)
{
	UserWidget->UpdateTimer(C4ProgressComp->GetGoal() - Seconds);
}

void AEXC4::NetDestroy()
{
	SetLifeSpan(5.f);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bExploded = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(AEXC4, bExploded, this);
	OnRep_Exploded();
}

void AEXC4::StartTimer(float Seconds)
{
	C4ProgressComp->ResetProgress();
	C4ProgressComp->SetGoal(Seconds);
	C4ProgressComp->StartProgress(Cast<AEXPlayerController>(GetInstigatorController()));
}

void AEXC4::OnRep_Exploded()
{
	if (!IsNetMode(NM_DedicatedServer))
	{
		const FVector Loc = GetActorLocation();
		FHitResult Hit;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		bool bComponentHit = GetWorld()->LineTraceSingleByChannel(Hit, Loc + FVector(0, 0, 10), Loc + FVector(0, 0, -11), ECC_WorldOnly, CollisionParams);
		UEXGameplayStatics::SpawnExplosion(GetWorld(), ExplosionClass, GetActorTransform(), bComponentHit, Hit);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const int32 bShowDebug = CVarShowC4Debug.GetValueOnGameThread();
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 32, FColor::Yellow, false, 3.f);
		}
#endif
	}
}

void AEXC4::OnDefused(AEXPlayerController* Player)
{
	C4ProgressComp->StopProgress(Player);
	Player->GetEXCharacter()->StopInteract(this, true);

	APlayerState* PS = Player ? Player->GetPlayerState<APlayerState>() : nullptr;
	
	GM->BroadcastLocalized(this, UEXObjectiveMessage::StaticClass(), 6, PS);

	Destroy();
}

bool AEXC4::CanInteract_Implementation(const AEXCharacter* Player, bool bLookAt) const
{
	if (!Super::CanInteract_Implementation(Player, bLookAt))
	{
		return false;
	}
	if (PlantingTeam == ETeam::Invalid)
	{
		return false;
	}
	const AEXPlayerState* const PlayerPS = Player->GetPlayerState<AEXPlayerState>();
	if (PlayerPS)
	{
		return (PlayerPS->GetTeam()->GetType() != PlantingTeam);
	}

	return false;
}

bool AEXC4::StartInteract_Implementation(AEXCharacter* Player)
{
	Super::StartInteract_Implementation(Player);

	Player->GetInventoryComponent()->EquipTool(RequiredTool, this);
	return true;
}

bool AEXC4::StopInteract_Implementation(AEXCharacter* Player)
{
	Super::StopInteract_Implementation(Player);

	Player->GetInventoryComponent()->Unequip(Player->GetInventoryComponent()->GetTool(RequiredTool), true);
	if (HasAuthority())
	{
		DefuseComp->StopProgress(Player->GetEXController());
	}
	return true;
}

void AEXC4::Reset()
{
	Destroy();
}

void AEXC4::StartProgress(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		const float ProgressModifier = Cast<IEXObjectiveTool>(Player->GetInventoryComponent()->GetTool(RequiredTool))->GetProgressModifier();
		DefuseComp->StartProgress(Player->GetEXController(), ProgressModifier);
	}
}

void AEXC4::SetTarget(AActor* InTarget)
{
	Target = InTarget;

	AEXGenerator* Gen = Cast<AEXGenerator>(Target);
	if (Gen)
	{
		Gen->SetC4(this);
	}
}

void AEXC4::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(AEXC4, PlantingTeam, PushReplicationParams::Default);
	DOREPLIFETIME_WITH_PARAMS_FAST(AEXC4, bExploded, PushReplicationParams::Default);
}

void AEXC4::SetProgressComp(UEXProgress* InProgressComp)
{
	Super::SetProgressComp(InProgressComp);

	C4ProgressComp = InProgressComp;
	if (HasAuthority())
	{
		C4ProgressComp->OnCompleted.AddUObject(this, &AEXC4::Explode);
	}
	else
	{
		C4ProgressComp->ProgressUpdate.AddUObject(this, &AEXC4::UpdateTimer);
		ProgressWidget->SetProgressComponent(C4ProgressComp);
	}
}


