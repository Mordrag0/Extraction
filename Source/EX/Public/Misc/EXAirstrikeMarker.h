// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXGrenade.h"
#include "EXAirstrikeMarker.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXAirstrikeMarker : public AEXGrenade
{
	GENERATED_BODY()
	
public:
	AEXAirstrikeMarker();

protected:

	void BeginPlay() override;

	void Explode() override;

	bool CheckIndoors() const;

	void CallAirStrike();


	UPROPERTY(EditDefaultsOnly, Category = "Air Strike")
	float IndoorCheckDistance = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Air Strike")
	float SpawnZ = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Air Strike")
	int32 NumRockets = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Air Strike")
	float DistBetweenRockets = 130.f;
	UPROPERTY(EditDefaultsOnly, Category = "Air Strike")
	float InitialProjectileVelocity = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Air Strike")
	TSubclassOf<class AEXRocket> RocketClass = nullptr;


};
