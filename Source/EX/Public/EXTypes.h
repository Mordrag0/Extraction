// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXTypes.generated.h"

class UMaterial;
class AEXPlayerState;

namespace MatchState
{
	extern ENGINE_API const FName EnteringMap;			// We are entering this map, actors are not yet ticking
	extern ENGINE_API const FName WaitingToStart;		// Actors are ticking, but the match has not yet started
	extern ENGINE_API const FName InProgress;			// Normal gameplay is occurring. Specific games will have their own state machine inside this state
	const FName PreRound = FName("PreRound");  			// A round ended
	const FName Overtime = FName("Overtime");       	// Current round is in overtime
	const FName PostRound = FName("PostRound");  		// A round ended
	extern ENGINE_API const FName WaitingPostMatch;		// Match has ended so we aren't accepting new players, but actors are still ticking
	extern ENGINE_API const FName LeavingMap;			// We are transitioning out of the map to another location
	extern ENGINE_API const FName Aborted;				// Match has failed due to network issues or other problems, cannot continue

	// If a game needs to add additional states, you may need to override HasMatchStarted and HasMatchEnded to deal with the new states
	// Do not add any states before WaitingToStart or after WaitingPostMatch
}

#define SURFACE_Default     SurfaceType_Default
#define SURFACE_Body		SurfaceType1
#define SURFACE_Head		SurfaceType2
#define SURFACE_Limb		SurfaceType3
#define SURFACE_Concrete	SurfaceType4
#define SURFACE_Dirt		SurfaceType5
#define SURFACE_Water		SurfaceType6
#define SURFACE_Metal		SurfaceType7
#define SURFACE_Grass		SurfaceType8
#define SURFACE_Glass		SurfaceType9
#define SURFACE_Wood		SurfaceType10
#define SURFACE_Asphalt		SurfaceType11
#define SURFACE_Brick		SurfaceType12
#define SURFACE_Bark		SurfaceType13
#define SURFACE_Dirt2		SurfaceType14
#define SURFACE_Gravel		SurfaceType15
#define SURFACE_Ice		    SurfaceType16
#define SURFACE_Mud		    SurfaceType17
#define SURFACE_Plastic		SurfaceType18
#define SURFACE_Rock		SurfaceType19
#define SURFACE_Sand		SurfaceType20
#define SURFACE_Snow		SurfaceType21
#define SURFACE_Soil		SurfaceType22
#define SURFACE_Vegetation	SurfaceType23
#define SURFACE_LowerLimb	SurfaceType24

UENUM()
enum class EWeaponInput : uint8 // Order matters for input priorities
{
	None,
	Primary,
	Secondary,
	Reload,
	ChangeFireMode,
};

USTRUCT()
struct FDecalData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	UMaterial* DecalMaterial = nullptr;

	/** Quad size (Width & Height) */
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float DecalSize = 256.f;

	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float LifeSpan = 10.f;

};

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Invalid,
	Attack, 
	Defense, 
	Spectator, 
};

USTRUCT()
struct FEXMatchState
{
	GENERATED_BODY()

	FEXMatchState() 
	{
	}
	
	FEXMatchState(
		const FName& InState,
		float InStartTime,
		int32 InDuration
	)
	: State(InState)
	, StartTime(InStartTime)
	, Duration(InDuration)
{
}

	bool IsValid() const { return !State.IsEqual(""); }

	UPROPERTY()
	FName State = FName("");
	UPROPERTY()
	float StartTime = 0.f;
	UPROPERTY()
	int32 Duration = 0;
};

UENUM(BlueprintType, Meta = (ScriptName = "EVote"))
enum class EVote : uint8
{
	None,
	Kick,
	Shuffle,
	Surrender,
};

USTRUCT()
struct FVoteInfo
{
	GENERATED_BODY()

	UPROPERTY()
	EVote VoteType;
	UPROPERTY()
	uint8 NumYesVotes;
	UPROPERTY()
	uint8 NumNoVotes;
	UPROPERTY()
	uint8 VotersTotal;
	UPROPERTY()
	uint8 VotesRequired;
	UPROPERTY()
	int32 Option; // What are we voting for? (For example which player are we kicking?)
	UPROPERTY()
	bool bActive;
	UPROPERTY()
	uint8 Idx;
	UPROPERTY()
	AEXPlayerState* Instigator;

	UPROPERTY(NotReplicated)
	TArray<AEXPlayerState*> YesVoters;
	UPROPERTY(NotReplicated)
	TArray<AEXPlayerState*> NoVoters;
	int32 VoteDuration = 15;

	FVoteInfo() : VoteType(EVote::None) {}
	FVoteInfo(EVote InVoteType, uint8 InVotersTotal, int32 InOption, int8 InIdx, AEXPlayerState* InInstigator)
		: VoteType(InVoteType), NumYesVotes(0), NumNoVotes(0), VotersTotal(InVotersTotal), Option(InOption), bActive(true), Idx(InIdx), Instigator(InInstigator)
	{
		VotesRequired = VotersTotal / 2 + 1;
		YesVoters.Reserve(16);
		NoVoters.Reserve(16);
		ensure(VoteType != EVote::None);
	}

	uint8 GetRequiredVotes() const { return VotersTotal / 2 + 1; }
	bool Succeded() const { return YesVoters.Num() >= VotesRequired; }
	bool Failed() const { return NoVoters.Num() > (VotersTotal - VotesRequired); }

	bool AddVote(AEXPlayerState* PS, bool bVote)
	{
		if (YesVoters.Contains(PS) || NoVoters.Contains(PS))
		{
			return false;
		}

		if (bVote)
		{
			YesVoters.Add(PS);
			NumYesVotes = YesVoters.Num();
		}
		else
		{
			NoVoters.Add(PS);
			NumNoVotes = NoVoters.Num();
		}
		return true;
	}
	bool IsValid() const { return bActive; }
	void End() { bActive = false; }
};

USTRUCT()
struct FEXMatchStateSW : public FEXMatchState
{
	GENERATED_BODY()

	FEXMatchStateSW()
	{
	}	
	FEXMatchStateSW(
		const FName& InState,
		float InStartTime,
		int32 InDuration,
		uint8 InAttSpawnWaveDuration,
		uint8 InDefSpawnWaveDuration,
		uint8 InAttSpawnOffset,
		uint8 InDefSpawnOffset
	)
		: FEXMatchState(InState, InStartTime, InDuration)
		, AttSpawnWaveDuration(InAttSpawnWaveDuration)
		, DefSpawnWaveDuration(InDefSpawnWaveDuration)
		, AttSpawnOffset(InAttSpawnOffset)
		, DefSpawnOffset(InDefSpawnOffset)
	{
	}

	UPROPERTY()
	uint8 AttSpawnWaveDuration = 0;
	UPROPERTY()
	uint8 DefSpawnWaveDuration = 0;
	UPROPERTY()
	uint8 AttSpawnOffset = 0;
	UPROPERTY()
	uint8 DefSpawnOffset = 0;
};

USTRUCT()
struct FEXMatchStateDM : public FEXMatchState
{
	GENERATED_BODY()

	FEXMatchStateDM()
	{
	}	
	FEXMatchStateDM(
		const FName& InState,
		float InStartTime,
		int32 InDuration
	) : FEXMatchState(InState, InStartTime, InDuration)
	{
	}
};


UENUM()
enum class EInteractableState : uint8
{
	NotReached,
	Active,
	Passed,
};


UENUM(BlueprintType)
enum class EAugment : uint8
{
	Empty,
	Drilled,
	QuickDraw,
	Looter,
	Recycle,
	Mechanic,
	QuickEye,
	WallJumper,
	AugmentCount,
};



UENUM(BlueprintType)
enum class EChatDestination : uint8
{
	None,
	// You can chat with your friends from anywhere
	Friends,			// The chat should go to anyone on the server

	// These are lobby chat types
	Global,				// The chat should route to everyone on the server
	Match,				// The chat should route to everyone currently in my match lobby

	// These are general game chatting
	Lobby,				// The chat came in from a hub lobby and needs to go directly to a player
	Local,				// The chat is local to everyone on that server
	Team,				// The chat is for anyone with the same team num
	LocalQuickChat,		// The chat is local to everyone on that server
	TeamQuickChat,      // The chat is for anyone with the same team num
	Whisper,			// The chat is only for the person specified

	System, 			// This chat message is a system message
	MOTD,				// This chat message is a message of the day

	Instance,			// This is chat message from a player in an instance to everyone in the lobby

};


UENUM(BlueprintType)
enum class EScoreType : uint8
{
	Misc,
	Support,
	Objective,
	Combat,
	Max
};

UENUM(BlueprintType)
enum class EMerc : uint8
{
	Default,
	Assault,
	Medic,
	Engi,
	Support,
	Aura,
	Bushwhacker,
	Fletcher,
	Fragger,
	Nader,
	Proxy,
	Sawbonez,
	Skyhammer,
	Vassili,
	Max
};

UENUM(BlueprintType, Meta = (ScriptName = "EWeapon"))
enum class EWeapon : uint8
{
	None,
	Auto1,
	Auto2,
	Auto3,
	Burst1,
	Burst2,
	Single1,
	Single2,
	Sniper1,
	Sniper2,
	Pistol1,
	Pistol2,
	Pistol3,
	Pistol4,
	Shotgun1,
	Shotgun2,
	Max
};

UENUM(BlueprintType)
enum class EAbility : uint8
{
	None,
	FragNade,
	GrenadeLauncher,
	Martyrdom,
	Mine,
	Sticky,
	StickyDetonator,
	Turret,
	Medpack,
	HealingStation,
	Defib,
	Airstrike,
	AmmoPack,
	AmmoStation,
	HBS,
	SelfRevive,
	Max
};


UENUM(BlueprintType)
enum class EGameResult : uint8
{
	Draw,
	Win,
	Lose,
	InProgress,
};


UENUM(BlueprintType)
enum class EQuickUseType : uint8
{
	None,
	Equip,
	Use,
};


UENUM(BlueprintType)
enum class EEngiTool : uint8
{
	None,
	Repair,
	Defuse,
	Deliver,
	Plant,
	Max
};


UENUM(BlueprintType)
enum class EIconSize : uint8
{
	KillFeed,
	Small,
	Medium,
	Large,
	Max
};


// #MASTER
UENUM(BlueprintType)
enum class EMSResponse : uint8
{
	INVALID,
	OUTDATED_CLIENT,
	CURRENTLY_OFFLINE,
	GAME_RESULT_SUCCESS_SW,
	GAME_RESULT_SUCCESS_DM,
	CLIENT_INFO,
	NAME_CHANGE_SUCCESS,
	CLEAR_NAME_SUCCESS,
	SQUAD_CHANGE_SUCCESS,
	PLAYER_INFO,
	ASSAULT_COURSE_RESULT_SUCCESS,
	ASSAULT_COURSE_STATS,
	PROFILE_INFO,
	PREVIOUS_GAMES,
	GAME_STATS,
	CHANGE_SKIN_SUCCESS,
	INVENTORY,
	SHOP_ITEMS,
	BUY_SHOP_ITEM_SUCCESS,
	BUY_SHOP_ITEM_FAIL,
	ASSAULT_COURSE_INFO,
};
UENUM(BlueprintType)
enum class EMSRequest : uint8
{
	INVALID,
	GAME_RESULT_SW,
	GAME_RESULT_DM,
	NAME_CHANGE,
	CLEAR_NAME,
	CLIENT_INFO,
	SQUAD_CHANGE,
	PLAYER_INFO,
	ASSAULT_COURSE_RESULT,
	ASSAULT_COURSE_STATS,
	PROFILE_INFO,
	PREVIOUS_GAMES,
	GAME_STATS,
	CHANGE_SKIN,
	INVENTORY,
	SHOP_ITEMS,
	BUY_SHOP_ITEM,
	ASSAULT_COURSE_INFO,
};



// Used for UI
UENUM()
enum class EMercStatus : uint8
{
	None,
	Unknown,
	Alive,
	Downed,
	Gibbed,
};


UENUM()
enum class EReportReason : uint8
{
	None,
	Cheating,
	BadBehaviour,
};





UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Ready,
	Down,
	Equipping,
	Unequipping,
	Firing,
	Reloading,
	Holding, // For abilities that are used in 2 parts (hold and release)
	Max UMETA(Hidden)
};

// Used primarily for animations
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Rifle,
	Pistol,
	Melee,
	FragNade,
	Medpack,
	Defib,
	Repair,
	RepairFast,
	Defuse,
	DefuseFast,
	Plant,
	PlantFast,
	Other,
};


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Body,
	Weapon,
	Ability,
	Other
};






/** 
* #MasterServer
* 
* Below are structs that are sent between the game and the master server.
* If we want to be able to send a struct to the master server we need to create a function specialization for FEXSerialize::Set 
* and if we want to be able to receive it from the master server we need to create a function specialization for FEXSerialize::Get.
* Inside those functions we need to call Set/Get on each member we want to be included
*/



// Objective times for a single stage, basically a TArray but we need this because we can't have 2D TArrays
USTRUCT(BlueprintType)
struct FObjectiveScores
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TArray<float> Score;

	int32 Num() const { return Score.Num(); }
	void Add(float Time) { Score.Add(Time); }
	float operator[](int32 Idx) const { return Score[Idx]; }
};


struct FWeaponStats // #EXTODO unite
{
	float PlayTime = 0.f;
	uint64 Uses = 0;
	uint64 Hits = 0;
	uint64 Headshots = 0;
	uint64 Kills = 0;
	float Damage = 0.f;
};
struct FWeaponStatsGame
{
	float PlayTime = 0.f;
	uint16 Uses = 0;
	uint16 Hits = 0;
	uint16 Headshots = 0;
	uint16 Kills = 0;
	float Damage = 0.f;
};

struct FAbilityStats
{
	float PlayTime = 0.f;
	uint64 Uses = 0;
	uint64 Hits = 0;
	uint64 Headshots = 0;
	uint64 Kills = 0;
	float Damage = 0.f;

};
struct FAbilityStatsGame
{
	float PlayTime = 0.f;
	uint16 Uses = 0;
	uint16 Hits = 0;
	uint16 Headshots = 0;
	uint16 Kills = 0;
	float Damage = 0.f;

};

struct FMercStats
{
	TMap<EWeapon, FWeaponStats> WeaponStats;
	TMap<EAbility, FAbilityStats> AbilityStats;
	uint64 Revives = 0;
	uint64 Deaths = 0;
	uint64 Assists = 0;
	float PlayTime = 0.f;
	uint64 HealthGiven = 0;
	uint64 AmmoGiven = 0;
	uint64 ExpSupport = 0;
	uint64 ExpCombat = 0;
	uint64 ExpObjective = 0;
	double DamageTaken = 0.f;
};
struct FMercStatsGame
{
	TMap<EWeapon, FWeaponStatsGame> WeaponStats;
	TMap<EAbility, FAbilityStatsGame> AbilityStats;
	TArray<EAugment> Augments;
	uint16 Revives = 0;
	uint16 Deaths = 0;
	uint16 Assists = 0;
	float PlayTime = 0.f;
	uint16 HealthGiven = 0;
	uint16 AmmoGiven = 0;
	uint16 ExpSupport = 0;
	uint16 ExpCombat = 0;
	uint16 ExpObjective = 0;
	double DamageTaken = 0.f;
};

USTRUCT()
struct FPlayerStats
{
	GENERATED_BODY()

	uint64 SteamId;
	TMap<EMerc, FMercStatsGame> MercStats;
	int64 Credits;
	FString Name;

	void CalculateCredits();

	// Transient - never sent to the master server
	void CalculateTransient();
	uint16 Kills;
	uint16 Deaths;
	uint16 Assists;
	uint16 TotalExp;
};

USTRUCT()
struct FSquadWeapon
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	EWeapon Weapon;
	UPROPERTY(Transient)
	uint32 Skin;
};
USTRUCT()
struct FSquadAbility
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	EAbility Ability;
	UPROPERTY(Transient)
	uint32 Skin;
};
USTRUCT()
struct FSquadMerc
{
	GENERATED_BODY()

	FSquadMerc() : Type(EMerc::Default), Skin(0) {};
	FSquadMerc(EMerc InType, TArray<EAugment> InSelectedAugments) : Type(InType), SelectedAugments(InSelectedAugments) {};

	UPROPERTY(Transient)
	EMerc Type;
	UPROPERTY(Transient)
	TArray<EWeapon> SelectedWeapons;
	UPROPERTY(Transient)
	TArray<EAugment> SelectedAugments;
	UPROPERTY(Transient)
	uint32 Skin;
	UPROPERTY(Transient)
	TArray<FSquadWeapon> Weapons;
	UPROPERTY(Transient)
	TArray<FSquadAbility> Abilities;

#ifdef WITH_EDITOR
	// #EDITORDEFAULTS
	static FSquadMerc CreateDefault(int MercIdx) 
	{
		FSquadMerc Merc;

		switch (MercIdx)
		{
		case 0:
			Merc.Type = EMerc::Sawbonez;
			break;
		case 1:
			Merc.Type = EMerc::Proxy;
			break;
		default:
			Merc.Type = EMerc::Fragger;
			break;
		}
		Merc.SelectedWeapons = { (EWeapon)0,(EWeapon)1,(EWeapon)2 };
		Merc.SelectedAugments = { (EAugment)1,(EAugment)2,(EAugment)3 };
		Merc.Skin = 0;
		for (int32 Idx = 0; Idx < (int32)EWeapon::Max; Idx++)
		{
			FSquadWeapon Weapon;
			Weapon.Weapon = (EWeapon)Idx;
			Merc.Weapons.Add(Weapon);
		}
		for (int32 Idx = 0; Idx < (int32)EAbility::Max; Idx++)
		{
			FSquadAbility Ability;
			Ability.Ability = (EAbility)Idx;
			Merc.Abilities.Add(Ability);
		}

		return Merc;
	}
#endif
};

struct FSquad
{
	FSquad() {};
	FSquad(TArray<EMerc> InSelectedMercs, TArray<FSquadMerc> InMercs) : SelectedMercs(InSelectedMercs), Mercs(InMercs) {};

	FSquadMerc GetMerc(EMerc Type) const;
	void SwapAugments(EMerc MercType, TArray<EAugment> InAugments);
	void ReplaceMerc(EMerc OldMerc, EMerc NewMerc);

	TArray<EMerc> SelectedMercs;
	TArray<FSquadMerc> Mercs;
};

struct FBasicInfo
{
	bool bNewlyCreated;
	bool bHasCustomName;
	FName Name;
	uint64 Exp;
	FString CreationDate;
	FSquad Squad;
	uint64 SteamId;
	uint64 Credits;
};

USTRUCT(BlueprintType)
struct FRoundScore
{
	GENERATED_BODY()

	EGameResult WinsAgaints(const FRoundScore& Other) const;

	void ObjectiveCompleted(float Time, int32 Stage);

	void SetLastObjectiveProgress(float InProgress)
	{
		LastObjectiveProgress = InProgress;
	}

	uint8 CompletedObjectives = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Score")
	TArray<FObjectiveScores> ObjectiveScores;
	float LastObjectiveProgress = 0.f;
};

struct FTeamResult
{
	TArray<FPlayerStats> Players;
	FRoundScore ObjectiveTimes;
};

struct FGameInfo // For requesting info about previous games
{
	uint64 GameId;
	FString Map;
	EGameResult PlayerWon;
	FString Date;
	bool bRanked;
};

struct FGameResultSW
{
	FGameResultSW() {};
	FGameResultSW(bool bInRanked, FString InMap) : bRanked(bInRanked), Map(InMap) {}

	bool bRanked = false;
	FString Map;
	FTeamResult Att;
	FTeamResult Def;
	EGameResult AttGameResult;
};

struct FGameResultDM
{
	FGameResultDM() {};
	FGameResultDM(FString InMap, const TArray<FPlayerStats>& InPlayers) : Map(InMap), Players(InPlayers) {}

	FString Map;
	TArray<FPlayerStats> Players;
};

struct FAssaultCourseResult
{
	int32 Score;
	FString Map;

};

struct FAssaultCoursePlayerStat
{
	uint64 SteamId;
	FName Name;
	int32 Score;
};

struct FAssaultCourseStats
{
	TArray<FAssaultCoursePlayerStat> Stats;
};


struct FShopItem
{
	uint32 ItemId;
	uint16 Price;
};
