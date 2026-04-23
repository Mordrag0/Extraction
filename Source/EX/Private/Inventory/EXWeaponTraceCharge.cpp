// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXWeaponTraceCharge.h"
#include "Inventory/EXWeaponAction.h"
#include "Inventory/EXFireMode.h"

UEXWeaponTraceCharge::UEXWeaponTraceCharge()
{
	bShootOnFireReleased = true;
}

bool UEXWeaponTraceCharge::CanFire(EWeaponInput Mode) const
{
	if (!Super::CanFire(Mode))
	{
		return false;
	}
	return !bOverheated;
}

bool UEXWeaponTraceCharge::StopFire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!UEXWeapon::StopFire(Mode, WorldTimeOverride))
	{
		return false;
	}
	PerformTrace(Charge / ChargeMax);
	Charge = 0.f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Overheat, FTimerDelegate::CreateUObject(this, &UEXWeaponTraceCharge::FireComplete, Mode), BatteryMax, false);
	return true;
}

void UEXWeaponTraceCharge::Overheat()
{
	if (BatteryMax > 0)
	{
		bOverheated = true;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Overheat, this, &UEXWeaponTraceCharge::OverheatComplete, BatteryMax, false);
	}
}

void UEXWeaponTraceCharge::OverheatComplete()
{
	bOverheated = false;
}

void UEXWeaponTraceCharge::BeginPlay()
{
	Super::BeginPlay();

	Battery = BatteryMax;
}

bool UEXWeaponTraceCharge::Tick(float DeltaTime)
{
	bool bAnythingChanged = Super::Tick(DeltaTime);

	UEXFireMode* ActiveFireMode = GetActiveFireMode();
	if (!ActiveFireMode || !ActiveFireMode->IsInitialized())
	{
		return bAnythingChanged;
	}

	const float OldBattery = Battery;
	const float OldCharge = Charge;
	float DeltaCharge = ActiveFireMode->IsHeldDown() ? DeltaTime : -DeltaTime;
	if (BatteryMax > 0.f)
	{
		if (Battery + DeltaCharge >= BatteryMax)
		{
			Overheat();
		}
		Battery = FMath::Max(0.f, FMath::Min(BatteryMax, Battery + DeltaCharge));
	}
	Charge = FMath::Max(0.f, FMath::Min(ChargeMax, Charge + DeltaCharge));
	return bAnythingChanged || (OldBattery != Battery) || (OldCharge != Charge);
}
