// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeapon.h"
#include "EXRocketLauncher.generated.h"

class AEXRocket;

/**
 * 
 */
UCLASS()
class EX_API UEXRocketLauncher : public UEXWeapon
{
	GENERATED_BODY()

public:
	UEXRocketLauncher();

	virtual void PerformShot() override;

	virtual void BeginPlay() override;

	virtual bool Tick(float DeltaTime) override;

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_PerformShot(const FVector Loc, const FRotator Rot);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_Unreliable_SetHomingTarget(const FVector& Loc);


	UPROPERTY(EditDefaultsOnly, Category = "RocketLauncher")
	FVector ProjectileSpawnOffset = FVector(100, 0, 0);

	UPROPERTY(EditDefaultsOnly, Category = "RocketLauncher")
	TSubclassOf<AEXRocket> RocketClass;

	UPROPERTY(EditDefaultsOnly, Category = "RocketLauncher")
	bool bHoming = true;

	UPROPERTY(EditDefaultsOnly, Category = "RocketLauncher")
	bool bTargetting = false;

	UFUNCTION(Client, Reliable)
	void Client_Reliable_SetHomingEnabled(bool bEnabled);

	UFUNCTION()
	void RocketExploded(AEXRocket* Rocket);

	UPROPERTY()
	TArray<AEXRocket*> HomingRockets;

	UPROPERTY(EditDefaultsOnly, Category = "RocketLauncher")
	float TraceRange = 10000.f;

	UPROPERTY()
	AEXCharacter* Player = nullptr;

};
