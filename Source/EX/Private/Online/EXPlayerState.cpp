// Fill out your copyright notice in the Description page of Project Settings.

#include "Online/EXPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Online/EXTeam.h"
#include "Player/EXPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EX.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "EXTypes.h"
#include "System/EXGameplayStatics.h"
#include "AdvancedSessionsLibrary.h"
#include "Online/EXGameStateSW.h"


AEXPlayerState::AEXPlayerState()
{
	for (int32 Idx = 0; Idx < (int32)EScoreType::Max; Idx++)
	{
		TempScores.Add((EScoreType)Idx, 0);
	}
}

void AEXPlayerState::ClientInitialize(class AController* C)
{
	Super::ClientInitialize(C);
}

FPlayerStats AEXPlayerState::GetStats()
{
	GameStats.Name = GetPlayerName();
	GameStats.SteamId = GetSteamID();
	GameStats.CalculateCredits();

	for (const FSquadMerc SquadMerc : Squad)
	{
		GameStats.MercStats.FindOrAdd(SquadMerc.Type).Augments = SquadMerc.SelectedAugments;
	}
	return GameStats;
}

void AEXPlayerState::SetSteamID(uint64 Id) 
{
	UE_LOG(LogEXPlayer, Error, TEXT("Player joined, steam id = '%llu'"), Id);
	SteamId = Id;
}

void AEXPlayerState::Client_Unreliable_AddScore_Implementation(int32 AddedScore, EScoreType Type)
{
	if (EXPC)
	{
		EXPC->OnScoreAdded(AddedScore, Type);
	}
}

void AEXPlayerState::Client_Reliable_ResetScore_Implementation(int32 OldPlayerScore)
{
	PlayerScore = OldPlayerScore;
}

void AEXPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (Team)
	{
		// Calling this on clients too, don't want to keep invalid PlayerStates in the team 
		// and we can keep the pointer to the team set in the PlayerState
		Team->RemoveFromTeam(this);
	}
	
}

void AEXPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	AEXPlayerState* PS = Cast<AEXPlayerState>(PlayerState);

	EXPC = PS->EXPC;
	EXPC->OverridePlayerState(this);
}

void AEXPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	AEXPlayerState* PS = Cast<AEXPlayerState>(PlayerState);
	PS->SetKills(Kills);
	PS->SetAssists(Assists);
	PS->SetDeaths(Deaths);
	PS->SetPlayerScore(PlayerScore);
	PS->SetRoundStats(RoundKills, RoundAssists, RoundDeaths, RoundPlayerScore, RoundGameStats);

	PS->SetSteamID(SteamId);
	PS->SetGameStats(GameStats);

	PS->EXPC = EXPC;
}

void AEXPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GS = GetWorld()->GetGameState<AEXGameStateBase>();
}

void AEXPlayerState::SetController(AEXPlayerController* InEXPC)
{
	EXPC = InEXPC;
	if (Team)
	{
		EXPC->OnTeamChanged(Team);
	}
}

void AEXPlayerState::SetTeam(AEXTeam* NewTeam)
{
	if(NewTeam)
	{
		UE_LOG(LogEXTeam, Log, TEXT("%s %s"), *GetName(), *NewTeam->GetName());
	}
	else
	{
		UE_LOG(LogEXTeam, Error, TEXT("NO TEAM"));
	}
	Team = NewTeam;

	if (EXPC)
	{
		EXPC->OnTeamChanged(Team);
		if (!IsNetMode(NM_DedicatedServer))
		{
			UEXGameplayStatics::RefreshTeamColors(this);
		}
	}
}

void AEXPlayerState::OnRep_Team(AEXTeam* OldTeam)
{
	if (OldTeam)
	{
		OldTeam->RemoveFromTeam(this);
	}
	if (Team)
	{
		Team->AddToTeam(this);
	}
	if (EXPC)
	{
		EXPC->OnTeamChanged(Team);
		if (!IsNetMode(NM_DedicatedServer))
		{
			UEXGameplayStatics::RefreshTeamColors(this);
		}
	}
}

void AEXPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	OnNameChanged.ExecuteIfBound(GetPlayerName());
}

void AEXPlayerState::SetRoundStats(int32 InKills, int32 InAssists, int32 InDeaths, int32 InScore, const FPlayerStats& InPlayerStats)
{
	RoundKills = InKills;
	RoundAssists = InAssists;
	RoundDeaths = InDeaths;
	RoundPlayerScore = InScore;
	RoundGameStats = InPlayerStats;
}

void AEXPlayerState::AddScore(int32 AddedScore, EScoreType ScoreType, bool bInstant /*= false*/)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	if (AddedScore != 0)
	{
		switch (ScoreType)
		{
			case EScoreType::Misc:
				break;
			case EScoreType::Support:
				GameStats.MercStats.FindOrAdd(CurrentMerc).ExpSupport += AddedScore;
				break;
			case EScoreType::Objective:
				GameStats.MercStats.FindOrAdd(CurrentMerc).ExpObjective += AddedScore;
				break;
			case EScoreType::Combat:
				GameStats.MercStats.FindOrAdd(CurrentMerc).ExpCombat += AddedScore;
				break;
			case EScoreType::Max:
			default:
				check(0);
				break;

		}
		PlayerScore += AddedScore;
		TempScores[ScoreType] += AddedScore;
		if (bInstant || (TempScores[ScoreType] >= 100)) 
		{
			Client_Unreliable_AddScore(TempScores[ScoreType], ScoreType);
			TempScores[ScoreType] = 0;
		} // #EXTODO timers - did i mean timers to show score on hud after a while if its not enough to be instant?
	}
}

void AEXPlayerState::Reset()
{
	Kills = 0;
	Assists = 0;
	Deaths = 0;
	PlayerScore = 0;

	GameStats = FPlayerStats{};
	Client_Reliable_ResetScore(PlayerScore);

	for (TTuple<EScoreType, int32> TempScore : TempScores)
	{
		TempScore.Value = 0;
	}

	Super::Reset();
}

bool AEXPlayerState::OnSameTeam(const AEXPlayerState* Other) const
{
	if (!Team || !Other->Team)
	{
		return false;
	}
	return GetTeam()->GetType() == Other->GetTeam()->GetType();
}

bool AEXPlayerState::IsTeam(ETeam InTeam) const
{
	return Team && (Team->GetType() == InTeam);
}

void AEXPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEXPlayerState, Kills);
	DOREPLIFETIME(AEXPlayerState, Deaths);
	DOREPLIFETIME(AEXPlayerState, Assists);
	// Owner will get score updated separately along with the type of the score, so don't need to replicate here
	DOREPLIFETIME(AEXPlayerState, PlayerScore);
	DOREPLIFETIME(AEXPlayerState, Team);

	DOREPLIFETIME(AEXPlayerState, SteamId);
	DOREPLIFETIME(AEXPlayerState, Squad);
}

void AEXPlayerState::MercStarted(EMerc Merc)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	check(Merc != EMerc::Default);
	CurrentMerc = Merc;
	MercSelectTime = GetWorld()->GetTimeSeconds();
}

void AEXPlayerState::MercStopped()
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	GameStats.MercStats.FindOrAdd(CurrentMerc).PlayTime += GetWorld()->GetTimeSeconds() - MercSelectTime;
}

void AEXPlayerState::WeaponEquipped(EWeapon Weapon)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	CurrentWeapon = Weapon;
	WeaponSelectTime = GetWorld()->GetTimeSeconds();
}

void AEXPlayerState::WeaponUnequipped()
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	GameStats.MercStats.FindOrAdd(CurrentMerc).WeaponStats.FindOrAdd(CurrentWeapon).PlayTime += GetWorld()->GetTimeSeconds() - WeaponSelectTime;
}

void AEXPlayerState::AbilityEquipped(EAbility Ability)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	CurrentAbility = Ability;
	AbilitySelectTime = GetWorld()->GetTimeSeconds();
}

void AEXPlayerState::AbilityUnequipped()
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	GameStats.MercStats.FindOrAdd(CurrentMerc).AbilityStats.FindOrAdd(CurrentAbility).PlayTime += GetWorld()->GetTimeSeconds() - AbilitySelectTime;
}

void AEXPlayerState::ShotMissed(EMerc Merc, EWeapon Weapon)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	FWeaponStatsGame& WeaponStats = GameStats.MercStats.FindOrAdd(Merc).WeaponStats.FindOrAdd(Weapon);
	WeaponStats.Uses++;
	UE_LOG(LogEXWeapon, Log, TEXT("ShotMissed (%s, %s)"), *UEnum::GetValueAsString(Merc), *UEnum::GetValueAsString(Weapon));
}

void AEXPlayerState::ShotHit(EMerc Merc, EWeapon Weapon, bool bHeadshot, bool bKill, float Damage)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	FWeaponStatsGame& WeaponStats = GameStats.MercStats.FindOrAdd(Merc).WeaponStats.FindOrAdd(Weapon);
	++WeaponStats.Uses;
	++WeaponStats.Hits;
	WeaponStats.Damage += Damage;
	if (bHeadshot)
	{
		++WeaponStats.Headshots;
	}
	if (bKill)
	{
		++WeaponStats.Kills;
	}
	UE_LOG(LogEXWeapon, Log, TEXT("ShotHit (%s, %s, %s, %s, %f)"), *UEnum::GetValueAsString(Merc), *UEnum::GetValueAsString(Weapon),
		(bHeadshot ? TEXT("HS") : TEXT("_")), (bKill ? TEXT("Kill") : TEXT("_")), Damage);
}

void AEXPlayerState::AbilityUsed(EMerc Merc, EAbility Ability)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	FAbilityStatsGame& AbilityStats = GameStats.MercStats.FindOrAdd(Merc).AbilityStats.FindOrAdd(Ability);
	++AbilityStats.Uses;
}

void AEXPlayerState::AbilityHit(EMerc Merc, EAbility Ability, bool bHit, bool bHeadshot, bool bKill, float Damage)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	FAbilityStatsGame& AbilityStats = GameStats.MercStats.FindOrAdd(Merc).AbilityStats.FindOrAdd(Ability);
	++AbilityStats.Hits;
	AbilityStats.Damage += Damage;
	if (bHeadshot)
	{
		++AbilityStats.Headshots;
	}
	if (bKill)
	{
		++AbilityStats.Kills;
	}
}

void AEXPlayerState::AddRevive(EMerc Merc, float HealthGiven)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	++GameStats.MercStats.FindOrAdd(Merc).Revives;
}

void AEXPlayerState::AddKill(EMerc Merc)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	//++GameStats.MercStats.FindOrAdd(Merc).Kills;
	++Kills;
}

void AEXPlayerState::AddDeath(EMerc Merc)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	++GameStats.MercStats.FindOrAdd(Merc).Deaths;
	++Deaths;
}

void AEXPlayerState::AddAssist(EMerc Merc)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	++GameStats.MercStats.FindOrAdd(Merc).Assists;
	++Assists;
}

void AEXPlayerState::DamageTaken(EMerc Merc, float Damage)
{
	if (!GS || !GS->IsMatchInProgress())
	{
		return;
	}
	GameStats.MercStats.FindOrAdd(Merc).DamageTaken += Damage;
}

void AEXPlayerState::StartRound()
{
	SetRoundStats(Kills, Assists, Deaths, PlayerScore, GameStats);
}

void AEXPlayerState::ResetRound()
{
	Kills = RoundKills;
	Assists = RoundAssists;
	Deaths = RoundDeaths;
	PlayerScore = RoundPlayerScore;
	GameStats = RoundGameStats;

	for (TTuple<EScoreType, int32> TempScore : TempScores)
	{
		TempScore.Value = 0;
	}

	Client_Reliable_ResetScore(PlayerScore);
}

void AEXPlayerState::SetSquad(const TArray<FSquadMerc>& InSquad)
{
	Squad = InSquad;
}

TArray<EAugment> AEXPlayerState::GetAugments(EMerc MercType) const
{
	for (const FSquadMerc& Merc : Squad)
	{
		if (Merc.Type == MercType)
		{
			return Merc.SelectedAugments;
		}
	}
	return TArray<EAugment>();
}


