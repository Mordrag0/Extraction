// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXWeaponTraceCharge.h"
#include "EXDefib.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXDefib : public UEXWeaponTraceCharge
{
	GENERATED_BODY()
	
public:
	UEXDefib();

	virtual void Server_Unreliable_TraceHit_Implementation(const FHitResult& HitResult, float ShotCharge) override;


};
