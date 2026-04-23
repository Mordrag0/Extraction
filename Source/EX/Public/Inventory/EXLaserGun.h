// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EXWeaponTrace.h"
#include "EXLaserGun.generated.h"

class AEXLaser;

/**
 * 
 */
UCLASS()
class EX_API UEXLaserGun : public UEXWeaponTrace
{
	GENERATED_BODY()
	
public:
	UEXLaserGun();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_Reliable_SpawnLaser(FVector Location);
	UFUNCTION(Server, Reliable)
	void Server_Reliable_MoveLaser(FVector Location);


	virtual void PerformShot() override;

	virtual bool Fire(EWeaponInput Mode, float WorldTimeOverride) override;

	virtual bool StopFire(EWeaponInput Mode, float WorldTimeOverride) override;

	virtual bool CanFire(EWeaponInput Mode) const override;

protected:

	bool GetTargetLocation(FVector& Location);

	void SpawnLaser();
	UPROPERTY(EditDefaultsOnly, Category = "LaserGun")
	float TraceDistance = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "LaserGun")
	float TimeToSpawnLaser = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "LaserGun")
	TSubclassOf<AEXLaser> LaserClass = nullptr;

	UPROPERTY(Replicated)
	class AEXLaser* Laser = nullptr;

	FTimerHandle TimerHandle_SpawnLaser;
private:
	void MoveLaser(FVector Location);
};
