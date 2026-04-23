// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EXTypes.h"
#include "Components/PanelWidget.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/GameplayStatics.h"
#include "EX.h"
#include "EXGameplayStatics.generated.h"

class UEXPersistentUser;
class AEXCharacter;
class UEXInventory;
class UEXWeapon;
class AEXLevelRules;

/**
 * 
 */
UCLASS()
class EX_API UEXGameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Team")
	static bool SameTeam(AEXCharacter* EXCharacter1, AEXCharacter* EXCharacter2);

	UFUNCTION(BlueprintCallable, Category = "Team")
	static TArray<AEXCharacter*> GetTeammates(AEXCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Team")
	static void RefreshTeamColors(UObject* WCO, bool bTest = false);

	UFUNCTION(BlueprintCallable, Category = "Team")
	static void RefreshCharacterColors(AEXTeam* Attackers, AEXTeam* Defenders);

	UFUNCTION(BlueprintCallable, Category = "Collision")
	static bool GetClosestPointOnCollision(const AActor* Actor, const FVector& Point, const ECollisionChannel& Channel, FVector& ClosestPoint);

	UFUNCTION(BlueprintCallable, Category = "Collision")
	static AActor* GetActorLookingAt(const AEXCharacter* Player, float MaxTraceRange);

	template<typename T>
	static void GetAllActorsOfClass(UObject* WCO, TSubclassOf<T> StaticClass, TArray<T*>& OutActors);

	static class AEXMusicManager* GetMusicManager(UObject* WCO);

	UFUNCTION(BlueprintCallable, Category = "FX")
	static void SpawnExplosion(class UWorld* World, TSubclassOf<class AEXExplosionEffect> ExplosionClass, const FTransform Transform, bool bComponentHit, const FHitResult& Impact);

	UFUNCTION(BlueprintCallable, Category = "Input")
	static UEXInputInfo* GetInputInfoCDO();

	static FString SecondsToText(int32 Seconds);
	static FString LeadingZeroes(int32 Val);


	/// Networking
	static uint64 GetLocalSteamID(UObject* WCO = nullptr);
	static FString GetSteamName(UObject* WCO = nullptr);

	static FBPUniqueNetId CreateSteamId(uint64 Id);

	static bool SteamOnline();

	///

	UFUNCTION(BlueprintPure, Category = "Static")
	static UEXWeapon* GetWeaponCDO(EWeapon WeaponType);
	UFUNCTION(BlueprintPure, Category = "Static")
	static UEXInventory* GetAbilityCDO(EAbility AbilityType);
	UFUNCTION(BlueprintPure, Category = "Static")
	static AEXCharacter* GetMercCDO(EMerc MercType);

	static TArray<TSubclassOf<AEXCharacter>> GetSquad(const TArray<EMerc>& Squad);

	static AEXLevelRules* GetLevelRules(UObject* WCO);
	static void SetLevelRules(AEXLevelRules* InLevelRules);
	static void SetPersistentUser(UEXPersistentUser* InPersistentUser);
	UFUNCTION(BlueprintPure, Category = "Static")
	static float GetMasterVolume() { return MasterVolume; }
	UFUNCTION(BlueprintPure, Category = "Static")
	static float GetMusicVolume() { return MusicVolume; }
	/// UI
	template <typename T>
	static TArray<T*> GetPanelChildren(UPanelWidget* Panel, bool bRecursive = false);
	///

	static void SetMasterVolume(float InMasterVolume);

	static void SetMusicVolume(float InMusicVolume);
private:
#if !WITH_EDITOR
	static AEXLevelRules* LevelRules;
#endif
	static float MasterVolume; // #EXTODO create sound class OR move to sound class (music manager?)
	static float MusicVolume;

	static void RefreshCharacterColors(AEXTeam* Team);
public:
	static FText FloatToText(float Value, int Precision);

	void ApplyRadialDamage(const UObject* WCO, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser = nullptr, AController* InstigatedByController = nullptr, bool bDoFullDamage = false, ECollisionChannel DamagePreventionChannel = ECC_Visibility_Simple);
};

template<typename T>
static void
UEXGameplayStatics::GetAllActorsOfClass(UObject* WCO, TSubclassOf<T> StaticClass, TArray<T*>& OutActors)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(WCO, StaticClass, Actors);
	OutActors.Empty(Actors.Num());
	for (AActor* Actor : Actors)
	{
		OutActors.Add(Cast<T>(Actor));
	}
}

template <typename T>
static TArray<T*>
UEXGameplayStatics::GetPanelChildren(UPanelWidget* Panel, bool bRecursive /*= false*/)
{
	TArray<T*> Ret;
	if (!Panel)
	{
		return Ret;
	}

	TArray<UWidget*> PanelChildren = Panel->GetAllChildren();
	for (UWidget* Child : PanelChildren)
	{
		T* RetChild = Cast<T>(Child);
		if (RetChild)
		{
			Ret.Add(RetChild);
		}
		else if (bRecursive)
		{
			UPanelWidget* ChildPanel = Cast<UPanelWidget>(Child);
			if (ChildPanel)
			{
				Ret.Append(GetPanelChildren<T>(ChildPanel, true));
			}
		}
	}

	return Ret;
}

