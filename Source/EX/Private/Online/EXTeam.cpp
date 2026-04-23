// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/EXTeam.h"
#include "Online/EXPlayerState.h"
#include "Misc/EXSpawnArea.h"
#include "GameFramework/PlayerStart.h"
#include "PLayer/EXPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "EXTypes.h"
#include "HUD/EXGameStatus.h"
#include "System/EXGameplayStatics.h"
#include "Online/EXLevelRules.h"
#include "Online/EXGameModeBase.h"


AEXTeam::AEXTeam()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AEXTeam::BeginPlay()
{
	Super::BeginPlay();

	AEXGameModeBase* GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
	if (GM)
	{
		MaxMembers = (int32)GM->GetMaxPlayersPerTeam();
	}
	UEXGameplayStatics::RefreshTeamColors(this);
}

void AEXTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXTeam, Type);
}

void AEXTeam::SetSpawnArea(AEXSpawnArea* SpawnArea)
{
	if (SpawnArea)
	{
		ActiveSpawnArea = SpawnArea;
	}
}

void AEXTeam::AddToTeam(AEXPlayerState* PS)
{
	if (HasAuthority())
	{
		AEXTeam* OldTeam = PS->GetTeam();
		if (OldTeam)
		{
			check(OldTeam != this && "Trying to add a player to a team that hes already on.");
			OldTeam->RemoveFromTeam(PS);
		}
		PS->SetTeam(this);
	}
	Members.Add(PS);
	UE_LOG(LogEXTeam, Verbose, TEXT("%s added to %s."), *PS->GetName(), *GetTeamName());
	OnChanged.Broadcast(this);
	UEXGameplayStatics::RefreshTeamColors(this);
}

void AEXTeam::RemoveFromTeam(AEXPlayerState* PS)
{
	if(HasAuthority())
	{
		AEXCharacter* Character = Cast<AEXCharacter>(PS->GetPawn());
		if (Character)
		{
			Character->SelfKill();
		}
		PS->SetTeam(nullptr);
	}
	if (Members.Contains(PS))
	{
		UE_LOG(LogEXTeam, Verbose, TEXT("%s removed from %s."), *PS->GetName(), *GetTeamName());
		Members.Remove(PS);
	}
	OnChanged.Broadcast(this);
	UEXGameplayStatics::RefreshTeamColors(this);
}

void AEXTeam::SetType(ETeam InType)
{
	Type = InType;
}

AActor* AEXTeam::GetPlayerStart(AEXPlayerController* Player)
{
	int32 Idx = Members.IndexOfByKey(Player->GetPlayerState<AEXPlayerState>());
	if (Idx == INDEX_NONE)
	{
		return nullptr;
	}
	return GetCurrentSpawnArea()->GetPlayerStart(Idx);
}

FString AEXTeam::GetTeamName() const
{
	const FString TeamName = IsAttacking() ? TEXT("Attackers") : IsDefending() ? TEXT("Defenders") : TEXT("Spectating");
	return TeamName;
}

void AEXTeam::OnRep_Type()
{
	AEXPlayerController* LocalPC = GetWorld()->GetFirstPlayerController<AEXPlayerController>();
	AEXPlayerState* LocalPS = LocalPC ? LocalPC->GetPlayerState<AEXPlayerState>() : nullptr;
	if(LocalPS && Members.Contains(LocalPS))
	{
		if (Members.Contains(LocalPS))
		{
			LocalPC->OnTeamChanged(this);
		}
	}
}



