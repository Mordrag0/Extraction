// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHost.h"
#include "EXOnlineBeaconHost.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXOnlineBeaconHost : public AOnlineBeaconHost
{
	GENERATED_BODY()


public:
	AEXOnlineBeaconHost();

	bool Start();

	void AddHost(AOnlineBeaconHostObject* HostObject);

	/** You can also remove a host if you so wish to as well */
	/** You can remove it with: UnregisterHost(const FString& BeaconType) */

protected:
	/** If we successfully started are not */
	bool bIsReady = false;
};
