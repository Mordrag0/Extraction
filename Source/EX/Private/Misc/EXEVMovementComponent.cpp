// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXEVMovementComponent.h"
#include "Misc/EXEV.h"
#include "Components/SplineComponent.h"
#include "Misc/EXBarricade.h"
#include "Player/EXCharacter.h"
#include "System/EXProgress.h"
#include "Components/SphereComponent.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"
#include "Online/EXGameStateSW.h"
#include "Misc/EXGenerator.h"
#include "Player/EXPlayerController.h"


UEXEVMovementComponent::UEXEVMovementComponent()
{
	SetIsReplicatedByDefault(true);
}

void UEXEVMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EV->HasAuthority())
	{
		AEXCharacter* Player = GetMovingPlayer();
		if (!!Player != IsMoving())
		{
			if (Player)
			{
				StartMoving(Player);
			}
			else
			{
				StopMoving();
			}
		}
		else
		{
			if (Player != CurrentMovingPlayer)
			{
				ChangeMovingPlayer(Player);
			}
		}
	}

	if (IsMoving())
	{
		TraveledDistance += Speed * DeltaTime;
		UpdatePosition(TraveledDistance, EV->GetMovingPlayer());
	}
}

void UEXEVMovementComponent::Init()
{
	EV = CastChecked<AEXEV>(GetOwner());
	Path = EV->GetPath();

	PathDistance = Path->GetDistanceAlongSplineAtSplinePoint(Path->GetNumberOfSplinePoints() - 1);

	EV->GetProgressComp()->SetGoal(PathDistance);
	SetClosestBarricade(EV->GetBarricade());

	GS = GetWorld()->GetGameState<AEXGameStateBase>();
}

void UEXEVMovementComponent::Multicast_Reliable_SetMoving_Implementation(bool bInMoving, bool bInBlocked, float CurrentDistance, float TimeStamp)
{
	if (IsAuthority())
	{
		return;
	}

	TraveledDistance = CurrentDistance;
	bMoving = bInMoving;
	bBlocked = bInBlocked;
	UpdatePosition(CurrentDistance, nullptr);
}

void UEXEVMovementComponent::StartMoving(AEXCharacter* Player)
{
	CurrentMovingPlayer = Player;
	bMoving = true;
	EV->GetProgressComp()->StartProgress(Player->GetEXController(), Speed);
	EV->SetObjectiveActive(true);

	Multicast_Reliable_SetMoving(bMoving, bBlocked, TraveledDistance, GS->GetServerWorldTimeSeconds());
}

void UEXEVMovementComponent::StopMoving()
{
	bMoving = false;
	EV->GetProgressComp()->StopProgress(CurrentMovingPlayer->GetEXController());
	EV->SetObjectiveActive(false);
	CurrentMovingPlayer = nullptr;

	Multicast_Reliable_SetMoving(bMoving, bBlocked, TraveledDistance, GS->GetServerWorldTimeSeconds());
}

void UEXEVMovementComponent::UpdatePosition(float Distance, AEXCharacter* Player)
{
	if (!HasBegunPlay()) // Path isn't initialized until BeginPlay has been called
	{
		return;
	}
	if (IsAuthority())
	{
		if (ClosestBarricade && (Distance >= ClosestBarricade->GetBlockingDistance())) // Check if we reached a barricade
		{
			if (ClosestBarricade->IsBarricadeActive())
			{
				Distance = ClosestBarricade->GetBlockingDistance();
				bBlocked = true;
				StopMoving();
			}
			else
			{
				ClosestBarricade->BarricadePassed(); // Disable repairing this barricade
				EV->BarricadePassed(); // Increase the barricade idx
				SetClosestBarricade(EV->GetBarricade()); // Get the next barricade if there is one
			}
		}
	}
	const FVector Loc = Path->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	const FRotator Rot = Path->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	EV->SetActorLocationAndRotation(Loc, Rot);
}

void UEXEVMovementComponent::Multicast_Reliable_Reset_Implementation()
{
	if (!IsAuthority())
	{
		Reset();
	}
}

AEXCharacter* UEXEVMovementComponent::GetMovingPlayer() const
{
	if (!EV->IsRepaired() || EV->IsFinished() || bBlocked)
	{
		return nullptr;
	}

	return EV->GetMovingPlayer();
}

void UEXEVMovementComponent::SetClosestBarricade(AEXBarricade* Barricade)
{
	if (ClosestBarricade)
	{
		AEXGenerator* OldGen = ClosestBarricade->GetGenerator();
		if (OldGen)
		{
			if (GetOwner()->HasAuthority())
			{
				OldGen->GetDestroyComp()->OnCompleted.Remove(DelegateHandle_BaricadeDestroyed);
			}
		}
	}
	ClosestBarricade = Barricade;
	if (ClosestBarricade)
	{
		AEXGenerator* Gen = ClosestBarricade->GetGenerator();
		if (Gen)
		{
			if (GetOwner()->HasAuthority())
			{
				DelegateHandle_BaricadeDestroyed = Gen->GetDestroyComp()->OnCompleted.AddUObject(this, &UEXEVMovementComponent::ClosestBarricadeDestroyed);
			}
		}
	}
}

bool UEXEVMovementComponent::IsAuthority() const
{
	return (EV && EV->HasAuthority());
}

void UEXEVMovementComponent::ClosestBarricadeDestroyed(AEXPlayerController* Player)
{
	bBlocked = false;
}

void UEXEVMovementComponent::Reset()
{
	Init();
	TraveledDistance = 0.f;
	UpdatePosition(0, nullptr);
	SetClosestBarricade(EV->GetBarricade());

	if (IsAuthority())
	{
		Multicast_Reliable_Reset();
	}
}

void UEXEVMovementComponent::ChangeMovingPlayer(AEXCharacter* Player)
{
	if (CurrentMovingPlayer) 
	{
		EV->GetProgressComp()->StopProgress(CurrentMovingPlayer->GetEXController());
	}
	CurrentMovingPlayer = Player;
	if (CurrentMovingPlayer) 
	{
		EV->GetProgressComp()->StartProgress(CurrentMovingPlayer->GetEXController(), Speed);
	}
}

