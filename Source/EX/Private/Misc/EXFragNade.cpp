// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXFragNade.h"

AEXFragNade::AEXFragNade()
{
	FuseTime = 0;
	bExplodeOnContact = false;
}

void AEXFragNade::SetTimeToExplode(float TimeToExplode)
{
	if (TimeToExplode <= 0.f)
	{
		Explode();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Explode, this, &AEXFragNade::Explode, TimeToExplode, false);
	}
}
