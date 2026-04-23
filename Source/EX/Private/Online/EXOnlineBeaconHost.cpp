// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXOnlineBeaconHost.h"
#include "EX.h"


AEXOnlineBeaconHost::AEXOnlineBeaconHost()
{
	BeaconState = EBeaconState::AllowRequests;
}

bool AEXOnlineBeaconHost::Start()
{
	//Call our init to start up the network interface
	bIsReady = InitHost();
	if (bIsReady)
	{
		UE_LOG(LogEXBeacon, Warning, TEXT("Beacon host initialized."));
	}
	else
	{
		UE_LOG(LogEXBeacon, Error, TEXT("Failed to initialize beacon host."));
	}
	return bIsReady;
}

void AEXOnlineBeaconHost::AddHost(AOnlineBeaconHostObject* HostObject)
{
	if (bIsReady)
	{
		RegisterHost(HostObject);
	}
}
