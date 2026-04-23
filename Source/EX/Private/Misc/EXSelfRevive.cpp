// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXSelfRevive.h"

UEXSelfRevive::UEXSelfRevive()
{
}

bool UEXSelfRevive::Fire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::Fire(Mode, WorldTimeOverride))
	{
		return false;
	}
	return true;
}

bool UEXSelfRevive::StopFire(EWeaponInput Mode, float WorldTimeOverride)
{
	if (!Super::StopFire(Mode, WorldTimeOverride))
	{
		return false;
	}
	return true;
}
