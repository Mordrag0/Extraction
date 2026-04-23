// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXOnlineBeaconHostObject.h"
#include "Online/EXOnlineBeaconClient.h"
#include "Online/EXGameModeBase.h"

AEXOnlineBeaconHostObject::AEXOnlineBeaconHostObject() 
{
	ClientBeaconActorClass = AEXOnlineBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();
}

bool AEXOnlineBeaconHostObject::Init()
{
	UE_LOG(LogBeacon, Verbose, TEXT("AEXOnlineBeaconHostObject Initialized"));
	return true;
}

void AEXOnlineBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	AEXOnlineBeaconClient* BeaconClient = Cast<AEXOnlineBeaconClient>(NewClientActor);
	if (BeaconClient)
	{
		BeaconClient->ClientPing();
	}
}

AOnlineBeaconClient* AEXOnlineBeaconHostObject::SpawnBeaconActor(UNetConnection* ClientConnection)
{
	const AEXGameModeBase* GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
	const bool bRanked = GM ? GM->IsRanked() : false;
	return (bRanked ? nullptr : Super::SpawnBeaconActor(ClientConnection));
}
