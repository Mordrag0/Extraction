// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EXShotgun.h"

void UEXShotgun::PerformShot()
{
	UEXWeapon::PerformShot();
	//if (FireValidated == 1) // #EXTODO
	{
		FireValidated = PelletsPerShot;
		for (uint8 Pellet = 0; Pellet < PelletsPerShot; Pellet++)
		{
			PerformTrace();
		}
	}
}
