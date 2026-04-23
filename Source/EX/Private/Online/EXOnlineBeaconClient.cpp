// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXOnlineBeaconClient.h"

void AEXOnlineBeaconClient::OnFailure()
{
	UE_LOG(LogBeacon, Verbose, TEXT("Test beacon connection failure, handling connection timeout."));
	Super::OnFailure();
}

void AEXOnlineBeaconClient::ClientPing_Implementation()
{
	UE_LOG(LogBeacon, Warning, TEXT("Ping"));
	//ServerPong();
}

void AEXOnlineBeaconClient::ServerPong_Implementation()
{
	UE_LOG(LogBeacon, Warning, TEXT("Pong"));
	ClientPing();
}

bool AEXOnlineBeaconClient::ServerPong_Validate()
{
	return true;
}

void AEXOnlineBeaconClient::Disconnect()
{
	DestroyBeacon();
}

bool AEXOnlineBeaconClient::Start(FString Address, int32 Port, bool PortOverride)
{
	//Address must be an IP or valid domain name such as epicgames.com or 127.0.0.1
	//Do not include a port in the address! Beacons use a different port then the standard 7777 for connection
	FURL Url(nullptr, *Address, ETravelType::TRAVEL_Absolute);

	if (PortOverride) //overriding it with a user specified port?
	{
		Url.Port = Port;
	}
	else //if not overriding just pull the config for it based on the beacon host ListenPort
	{
		int32 PortConfig;
		GConfig->GetInt(TEXT("/Script/OnlineSubsystemUtils.OnlineBeaconHost"), TEXT("ListenPort"), PortConfig, GEngineIni);
		Url.Port = PortConfig;
	}

	//Tell our beacon client to begin connection request to server address with our beacon port
	return InitClient(Url);
}

