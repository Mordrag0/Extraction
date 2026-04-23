// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "Engine/Canvas.h" 
#include "EXTypes.h"
#include "EXDamageType.generated.h"

USTRUCT(BlueprintType)
struct FEXPointDamageEvent : public FPointDamageEvent
{
	GENERATED_BODY()

	FEXPointDamageEvent() { }
	FEXPointDamageEvent(float InDamage, struct FHitResult const& InHitInfo, FVector const& InShotDirection, TSubclassOf<class UDamageType> InDamageTypeClass, EMerc InMercType)
		: FPointDamageEvent(InDamage, InHitInfo, InShotDirection, InDamageTypeClass), MercType(InMercType)
	{
	}

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 101;

	virtual int32 GetTypeID() const override { return FEXPointDamageEvent::ClassID; }

	virtual bool IsOfType(int32 InID) const override { return (FEXPointDamageEvent::ClassID == InID) || FPointDamageEvent::IsOfType(InID); }

	EMerc MercType = EMerc::Max;
};

USTRUCT(BlueprintType)
struct FEXRadialDamageEvent : public FRadialDamageEvent
{
	GENERATED_BODY()

	FEXRadialDamageEvent() {}
	FEXRadialDamageEvent(EMerc InMercType) : MercType(InMercType) {}

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 102;

	virtual int32 GetTypeID() const override { return FEXRadialDamageEvent::ClassID; }

	virtual bool IsOfType(int32 InID) const override { return (FEXRadialDamageEvent::ClassID == InID) || FRadialDamageEvent::IsOfType(InID); }

	EMerc MercType = EMerc::Max;
};

USTRUCT(BlueprintType)
struct FEXSelfKillDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	FEXSelfKillDamageEvent() : Damage(0.f) {}
	FEXSelfKillDamageEvent(float InDamage, TSubclassOf<class UDamageType> InDamageTypeClass) : FDamageEvent(InDamageTypeClass), Damage(InDamage) {}

	float Damage = 0.f;

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 103;

	virtual int32 GetTypeID() const override { return FEXSelfKillDamageEvent::ClassID; }

	virtual bool IsOfType(int32 InID) const override { return (FEXSelfKillDamageEvent::ClassID == InID); }

};

USTRUCT(BlueprintType)
struct FEXFallDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	FEXFallDamageEvent() : Velocity(0.f) {}
	FEXFallDamageEvent(float InVelocity, TSubclassOf<class UDamageType> InDamageTypeClass) : FDamageEvent(InDamageTypeClass), Velocity(InVelocity) {}

	float Velocity = 0.f;

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 104;

	virtual int32 GetTypeID() const override { return FEXSelfKillDamageEvent::ClassID; }

	virtual bool IsOfType(int32 InID) const override { return (FEXSelfKillDamageEvent::ClassID == InID); }

};

/**
 * 
 */
UCLASS(const, Blueprintable, BlueprintType)
class EX_API UEXDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	// Icon displayed in death messages log when killed with this weapon 
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	UTexture2D* KillIcon = nullptr;

	/** this is the name that will be used for the {WeaponName} message option*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Messages)
	FText AssociatedWeaponName;
};
