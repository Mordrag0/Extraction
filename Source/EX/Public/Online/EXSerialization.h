// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EX.h"
#include "EXTypes.h"
#include "LowEntryExtendedStandardLibrary.h"

struct FEXSerialize
{
public:
	template <typename... T>
	FEXSerialize(EMSRequest Request, T... Args)
	{
		UE_LOG(LogEXOnline, Warning, TEXT("%s"), *UEnum::GetValueAsString(Request));
		Set(Request);
		SetVData(Args...);
	}

	FEXSerialize(FEXSerialize&& Other)
	{
		Data = MoveTemp(Other.Data);
	}
	FEXSerialize& operator=(const FEXSerialize&) = default;
	FEXSerialize(const FEXSerialize&) = delete;

	TArray<uint8> GetData() const { return Data; }
private:

	template <typename T>
	void SetVData(T Value)
	{
		Set(Value);
	}
	template <typename T, typename... FArgs>
	void SetVData(T Value, FArgs... Args)
	{
		Set(Value);
		SetVData(Args...);
	}

	template<typename T>
	void Set(const T& V);
#pragma region Setters
	template <>
	void Set<FSquad>(const FSquad& V)
	{
		Set(V.SelectedMercs);
		Set(V.Mercs);
	}
	template <>
	void Set<FSquadWeapon>(const FSquadWeapon& V)
	{
		Set(V.Weapon);
		Set(V.Skin);
	}
	template <>
	void Set<FSquadAbility>(const FSquadAbility& V)
	{
		Set(V.Ability);
		Set(V.Skin);
	}
	template <>
	void Set<FSquadMerc>(const FSquadMerc& V)
	{
		Set(V.Type);
		Set(V.SelectedWeapons);
		Set(V.SelectedAugments);
		Set(V.Skin);
		Set(V.Weapons);
		Set(V.Abilities);
	}
	template <>
	void Set<FPlayerStats>(const FPlayerStats& V)
	{
		Set(V.SteamId);
		Set(V.MercStats);
		Set(V.Credits);
		// Do not set name
	}
	template <>
	void Set<FGameResultSW>(const FGameResultSW& V)
	{
		Set(V.bRanked);
		Set(V.Map);
		Set(V.Att);
		Set(V.Def);
		Set(V.AttGameResult);
	}
	template <>
	void Set<FGameResultDM>(const FGameResultDM& V)
	{
		Set(V.Map);
		Set(V.Players);
	}
	template <>
	void Set<FTeamResult>(const FTeamResult& V)
	{
		Set(V.Players);
		Set(V.ObjectiveTimes);
	}
	template <>
	void Set<FWeaponStats>(const FWeaponStats& V)
	{
		Set(V.PlayTime);
		Set(V.Uses);
		Set(V.Hits);
		Set(V.Headshots);
		Set(V.Kills);
		Set(V.Damage);
	}
	template <>
	void Set<FWeaponStatsGame>(const FWeaponStatsGame& V)
	{
		Set(V.PlayTime);
		Set(V.Uses);
		Set(V.Hits);
		Set(V.Headshots);
		Set(V.Kills);
		Set(V.Damage);
	}
	template <>
	void Set<FAbilityStats>(const FAbilityStats& V)
	{
		Set(V.PlayTime);
		Set(V.Uses);
		Set(V.Kills);
		Set(V.Hits);
		Set(V.Headshots);
		Set(V.Damage);
	}
	template <>
	void Set<FAbilityStatsGame>(const FAbilityStatsGame& V)
	{
		Set(V.PlayTime);
		Set(V.Uses);
		Set(V.Kills);
		Set(V.Hits);
		Set(V.Headshots);
		Set(V.Damage);
	}
	template <>
	void Set<FMercStats>(const FMercStats& V)
	{
		Set(V.WeaponStats);
		Set(V.AbilityStats);
		Set(V.Revives);
		Set(V.Deaths);
		Set(V.Assists);
		Set(V.PlayTime);
		Set(V.HealthGiven);
		Set(V.AmmoGiven);
		Set(V.ExpSupport);
		Set(V.ExpCombat);
		Set(V.ExpObjective);
		Set(V.DamageTaken);
	}
	template <>
	void Set<FMercStatsGame>(const FMercStatsGame& V)
	{
		Set(V.WeaponStats);
		Set(V.AbilityStats);
		Set(V.Augments);
		Set(V.Revives);
		Set(V.Deaths);
		Set(V.Assists);
		Set(V.PlayTime);
		Set(V.HealthGiven);
		Set(V.AmmoGiven);
		Set(V.ExpSupport);
		Set(V.ExpCombat);
		Set(V.ExpObjective);
		Set(V.DamageTaken);
	}
	template <>
	void Set<FRoundScore>(const FRoundScore& V)
	{
		Set(V.CompletedObjectives);
		Set(V.ObjectiveScores);
		Set(V.LastObjectiveProgress);
	}
	template <>
	void Set<FObjectiveScores>(const FObjectiveScores& V)
	{
		Set(V.Score);
	}
	template <>
	void Set<FAssaultCourseResult>(const FAssaultCourseResult& V)
	{
		Set(V.Score);
		Set(V.Map);
	}
	template <>
	void Set<FAssaultCoursePlayerStat>(const FAssaultCoursePlayerStat& V)
	{
		Set(V.SteamId);
		Set(V.Name);
		Set(V.Score);
	}
	template <>
	void Set<FAssaultCourseStats>(const FAssaultCourseStats& V)
	{
		Set(V.Stats);
	}
#pragma endregion Setters

#pragma region Primitives
	template<>
	void Set<bool>(const bool& V);
	template<>
	void Set<uint8>(const uint8& V);
	template<>
	void Set<int16>(const int16& V);
	template<>
	void Set<uint16>(const uint16& V);
	template<>
	void Set<int32>(const int32& V);
	template<>
	void Set<uint32>(const uint32& V);
	template<>
	void Set<int64>(const int64& V);
	template<>
	void Set<uint64>(const uint64& V);
	template<>
	void Set<float>(const float& V);
	template<>
	void Set<double>(const double& V);
	template<>
	void Set<FString>(const FString& V);
	template<>
	void Set<FName>(const FName& V);
	template<typename T, typename Size = uint8>
	void Set(const TArray<T>& V);
	template<typename KeyType, typename ValueType, typename Size = uint8>
	void Set(const TMap<KeyType, ValueType>& V);
	template<typename T, typename Size = uint8>
	void Set(const TSet<T>& V);
#pragma endregion Primitives
	TArray<uint8> Data;
};

struct FEXUnserialize
{
	FEXUnserialize(const TArray<uint8>& InData)
	{
		Data = InData;
	}
	FEXUnserialize(FEXUnserialize&& Other)
	{
		Data = MoveTemp(Other.Data);
		UnserializeIdx = Other.UnserializeIdx;
	}
	FEXUnserialize& operator=(const FEXUnserialize&) = default;
	FEXUnserialize(const FEXUnserialize&) = delete;
	EMSResponse GetRequest() 
	{ 
		EMSResponse Request;
		Get(Request);
		return Request;
	}
	bool HasData() const
	{
		return UnserializeIdx < (uint32)Data.Num();
	}

private:
	template <typename T>
	void Get(T& V);
public:
#pragma region Getters
	template <>
	void Get<FBasicInfo>(FBasicInfo& V)
	{
		Get(V.bNewlyCreated);
		Get(V.bHasCustomName);
		Get(V.Name);
		Get(V.Exp);
		Get(V.Squad);
		Get(V.CreationDate);
		Get(V.SteamId);
		Get(V.Credits);
	}
	template <>
	void Get<FSquad>(FSquad& V)
	{
		Get(V.SelectedMercs);
		Get(V.Mercs);
	}
	template <>
	void Get<FSquadWeapon>(FSquadWeapon& V)
	{
		Get(V.Weapon);
		Get(V.Skin);
	}
	template <>
	void Get<FSquadAbility>(FSquadAbility& V)
	{
		Get(V.Ability);
		Get(V.Skin);
	}
	template <>
	void Get<FSquadMerc>(FSquadMerc& V)
	{
		Get(V.Type);
		Get(V.SelectedWeapons);
		Get(V.SelectedAugments);
		Get(V.Skin);
		Get(V.Weapons);
		Get(V.Abilities);
	}
	template <>
	void Get<FPlayerStats>(FPlayerStats& V)
	{
		Get(V.SteamId);
		Get(V.MercStats);
		Get(V.Credits);
		Get(V.Name);
		V.CalculateTransient();
	}
	template <>
	void Get<FGameInfo>(FGameInfo& V)
	{
		Get(V.GameId);
		Get(V.Map);
		Get(V.PlayerWon);
		Get(V.Date);
		Get(V.bRanked);
	}
	template <>
	void Get<FGameResultSW>(FGameResultSW& V)
	{
		Get(V.bRanked);
		Get(V.Map);
		Get(V.Att);
		Get(V.Def);
		Get(V.AttGameResult);
	}
	template <>
	void Get<FGameResultDM>(FGameResultDM& V)
	{
		Get(V.Map);
		Get(V.Players);
	}
	template <>
	void Get<FTeamResult>(FTeamResult& V)
	{
		Get(V.Players);
		Get(V.ObjectiveTimes);
	}
	template <>
	void Get<FWeaponStats>(FWeaponStats& V)
	{
		Get(V.PlayTime);
		Get(V.Uses);
		Get(V.Hits);
		Get(V.Headshots);
		Get(V.Kills);
		Get(V.Damage);
	}
	template <>
	void Get<FWeaponStatsGame>(FWeaponStatsGame& V)
	{
		Get(V.PlayTime);
		Get(V.Uses);
		Get(V.Hits);
		Get(V.Headshots);
		Get(V.Kills);
		Get(V.Damage);
	}
	template <>
	void Get<FAbilityStats>(FAbilityStats& V)
	{
		Get(V.PlayTime);
		Get(V.Uses);
		Get(V.Kills);
		Get(V.Hits);
		Get(V.Headshots);
		Get(V.Damage);
	}
	template <>
	void Get<FAbilityStatsGame>(FAbilityStatsGame& V)
	{
		Get(V.PlayTime);
		Get(V.Uses);
		Get(V.Kills);
		Get(V.Hits);
		Get(V.Headshots);
		Get(V.Damage);
	}
	template <>
	void Get<FMercStats>(FMercStats& V)
	{
		Get(V.WeaponStats);
		Get(V.AbilityStats);
		Get(V.Revives);
		Get(V.Deaths);
		Get(V.Assists);
		Get(V.PlayTime);
		Get(V.HealthGiven);
		Get(V.AmmoGiven);
		Get(V.ExpSupport);
		Get(V.ExpCombat);
		Get(V.ExpObjective);
		Get(V.DamageTaken);
	}
	template <>
	void Get<FMercStatsGame>(FMercStatsGame& V)
	{
		Get(V.WeaponStats);
		Get(V.AbilityStats);
		Get(V.Augments);
		Get(V.Revives);
		Get(V.Deaths);
		Get(V.Assists);
		Get(V.PlayTime);
		Get(V.HealthGiven);
		Get(V.AmmoGiven);
		Get(V.ExpSupport);
		Get(V.ExpCombat);
		Get(V.ExpObjective);
		Get(V.DamageTaken);
	}
	template <>
	void Get<FRoundScore>(FRoundScore& V)
	{
		Get(V.CompletedObjectives);
		Get(V.ObjectiveScores);
		Get(V.LastObjectiveProgress);
	}
	template <>
	void Get<FObjectiveScores>(FObjectiveScores& V)
	{
		Get(V.Score);
	}
	template <>
	void Get<FAssaultCourseResult>(FAssaultCourseResult& V)
	{
		Get(V.Score);
		Get(V.Map);
	}
	template <>
	void Get<FAssaultCoursePlayerStat>(FAssaultCoursePlayerStat& V)
	{
		Get(V.SteamId);
		Get(V.Name);
		Get(V.Score);
	}
	template <>
	void Get<FAssaultCourseStats>(FAssaultCourseStats& V)
	{
		Get(V.Stats);
	}
	template <>
	void Get<FShopItem>(FShopItem& V)
	{
		Get(V.ItemId);
		Get(V.Price);
	}
#pragma endregion Getters

#pragma region Primitives
	template<>
	void Get<bool>(bool& V);
	template<>
	void Get<uint8>(uint8& V);
	template<>
	void Get<int16>(int16& V);
	template<>
	void Get<uint16>(uint16& V);
	template<>
	void Get<int32>(int32& V);
	template<>
	void Get<uint32>(uint32& V);
	template<>
	void Get<int64>(int64& V);
	template<>
	void Get<uint64>(uint64& V);
	template<>
	void Get<FString>(FString& V);
	template<>
	void Get<FName>(FName& V);
	template<>
	void Get<float>(float& V);
	template<>
	void Get<double>(double& V);

	template<typename T, typename Size = uint8>
	void Get(TArray<T>& V);
	template<typename KeyType, typename ValueType, typename Size = uint8>
	void Get(TMap<KeyType, ValueType>& V);
	template<typename T, typename Size = uint8>
	void Get(TSet<T>& V);
#pragma endregion Primitives
private:
	uint32 UnserializeIdx = 0;
	TArray<uint8> Data;
};


#pragma region Containers
template<typename T, typename Size>
void FEXUnserialize::Get(TArray<T>& V)
{
	Size Count;
	Get<Size>(Count);
	V.Empty(Count);
	for (int Idx = 0; Idx < Count; Idx++)
	{
		T Element;
		Get(Element);
		V.Add(Element);
	}
}
template<typename KeyType, typename ValueType, typename Size>
void FEXUnserialize::Get(TMap<KeyType, ValueType>& V)
{
	Size Count;
	Get<Size>(Count);
	V.Empty(Count);
	for (int Idx = 0; Idx < Count; Idx++)
	{
		KeyType Key;
		Get<KeyType>(Key);
		ValueType Value;
		Get(Value);
		V.Add(Key, Value);
	}
}
template<typename T, typename Size>
void FEXUnserialize::Get(TSet<T>& V)
{
	Size Count;
	Get<Size>(Count);
	V.Empty(Count);
	for (int Idx = 0; Idx < Count; Idx++)
	{
		T Element;
		Get(Element);
		V.Add(Element);
	}
}
template<typename KeyType, typename ValueType, typename Size>
void FEXSerialize::Set(const TMap<KeyType, ValueType>& V)
{
	Set<Size>(V.Num());
	for (const auto& Pair : V)
	{
		Set<Size>(static_cast<Size>(Pair.Key));
		Set(Pair.Value);
	}
}

template<typename T, typename Size>
void FEXSerialize::Set(const TArray<T>& V)
{
	Set<Size>(static_cast<Size>(V.Num()));
	for (const auto& Value : V)
	{
		Set(Value);
	}
}

template<typename T, typename Size>
void FEXSerialize::Set(const TSet<T>& V)
{
	Set<Size>(static_cast<Size>(V.Num()));
	for (const auto& Value : V)
	{
		Set(Value);
	}
}
#pragma endregion Containers

template<typename T>
void FEXUnserialize::Get(T& V)
{
	V = static_cast<T>(Data[UnserializeIdx++]);
}

#pragma region Primitives
template<>
void FEXUnserialize::Get<bool>(bool& V)
{
	V = (Data[UnserializeIdx++] > 0);
}
template<>
void FEXUnserialize::Get<uint8>(uint8& V)
{
	V = Data[UnserializeIdx++];
}
template<>
void FEXUnserialize::Get<int16>(int16& V)
{
	V = (Data[UnserializeIdx++] << 8) | Data[UnserializeIdx++];
}
template<>
void FEXUnserialize::Get<uint16>(uint16& V)
{
	V = (Data[UnserializeIdx++] << 8) | Data[UnserializeIdx++];
}
template<>
void FEXUnserialize::Get<int32>(int32& V)
{
	V =
		  ((uint64)Data[UnserializeIdx++] << 24)
		| ((uint64)Data[UnserializeIdx++] << 16)
		| ((uint64)Data[UnserializeIdx++] << 8)
		| ((uint64)Data[UnserializeIdx++] << 0);
}
template<>
void FEXUnserialize::Get<uint32>(uint32& V)
{
	V =
		  ((uint64)Data[UnserializeIdx++] << 24)
		| ((uint64)Data[UnserializeIdx++] << 16)
		| ((uint64)Data[UnserializeIdx++] << 8)
		| ((uint64)Data[UnserializeIdx++] << 0);
}
template<>
void FEXUnserialize::Get<int64>(int64& V)
{
	V =
		  ((int64)Data[UnserializeIdx++] << 56)
		| ((int64)Data[UnserializeIdx++] << 48)
		| ((int64)Data[UnserializeIdx++] << 40)
		| ((int64)Data[UnserializeIdx++] << 32)
		| ((int64)Data[UnserializeIdx++] << 24)
		| ((int64)Data[UnserializeIdx++] << 16)
		| ((int64)Data[UnserializeIdx++] << 8)
		| ((int64)Data[UnserializeIdx++] << 0);
}
template<>
void FEXUnserialize::Get<uint64>(uint64& V)
{
	V =
		((uint64)Data[UnserializeIdx++] << 56)
		| ((uint64)Data[UnserializeIdx++] << 48)
		| ((uint64)Data[UnserializeIdx++] << 40)
		| ((uint64)Data[UnserializeIdx++] << 32)
		| ((uint64)Data[UnserializeIdx++] << 24)
		| ((uint64)Data[UnserializeIdx++] << 16)
		| ((uint64)Data[UnserializeIdx++] << 8)
		| ((uint64)Data[UnserializeIdx++] << 0);
}
template<>
void FEXUnserialize::Get<float>(float& V)
{
	uint16 Long;
	Get(Long);
	V = static_cast<float>(Long);
}
template<>
void FEXUnserialize::Get<double>(double& V)
{
	uint64 Long;
	Get(Long);
	V = static_cast<double>(Long);
}
template<>
void FEXUnserialize::Get<FString>(FString& V)
{
	uint8 Length;
	Get(Length);
	TArray<uint8> StrData;
	for (int Idx = 0; Idx < Length; Idx++)
	{
		uint8 Element;
		Get(Element);
		StrData.Add(Element);
	}
	V = ULowEntryExtendedStandardLibrary::BytesToStringUtf8(StrData);
}
template<>
void FEXUnserialize::Get<FName>(FName& V)
{
	FString Str = V.ToString();
	Get(Str);
	V = *Str;
}

template<typename T>
void FEXSerialize::Set(const T& V)
{
	Data.Add(static_cast<uint8>(V));
}

template<>
void FEXSerialize::Set<bool>(const bool& V)
{
	Data.Add(V ? 1 : 0);
}
template<>
void FEXSerialize::Set<uint8>(const uint8& V)
{
	Data.Add(V);
}
template<>
void FEXSerialize::Set<int16>(const int16& V)
{
	Data.Add(V >> 8);
	Data.Add(V);
}
template<>
void FEXSerialize::Set<uint16>(const uint16& V)
{
	Data.Add(V >> 8);
	Data.Add(V);
}
template<>
void FEXSerialize::Set<int32>(const int32& V)
{
	Data.Add(V >> 24);
	Data.Add(V >> 16);
	Data.Add(V >> 8);
	Data.Add(V);
}
template<>
void FEXSerialize::Set<uint32>(const uint32& V)
{
	Data.Add(V >> 24);
	Data.Add(V >> 16);
	Data.Add(V >> 8);
	Data.Add(V);
}
template<>
void FEXSerialize::Set<int64>(const int64& V)
{
	Data.Add(V >> 56);
	Data.Add(V >> 48);
	Data.Add(V >> 40);
	Data.Add(V >> 32);
	Data.Add(V >> 24);
	Data.Add(V >> 16);
	Data.Add(V >> 8);
	Data.Add(V);
}
template<>
void FEXSerialize::Set<uint64>(const uint64& V)
{
	Data.Add(V >> 56);
	Data.Add(V >> 48);
	Data.Add(V >> 40);
	Data.Add(V >> 32);
	Data.Add(V >> 24);
	Data.Add(V >> 16);
	Data.Add(V >> 8);
	Data.Add(V);
}
template<>
void FEXSerialize::Set<float>(const float& V)
{
	uint16 Val = FMath::RoundToNegativeInfinity(V);
	Set(Val);
}
template<>
void FEXSerialize::Set<double>(const double& V)
{
	uint64 Val = FMath::RoundToNegativeInfinity(V);
	Set(Val);
}
template<>
void FEXSerialize::Set<FString>(const FString& V)
{
	TArray<uint8> StrData = ULowEntryExtendedStandardLibrary::StringToBytesUtf8(V);
	Data.Add(StrData.Num());
	for (const uint8& Byte : StrData)
	{
		Data.Add(Byte);
	}
}
template<>
void FEXSerialize::Set<FName>(const FName& V)
{
	FString Str = V.ToString();
	Set(Str);
}
#pragma endregion Primitive




