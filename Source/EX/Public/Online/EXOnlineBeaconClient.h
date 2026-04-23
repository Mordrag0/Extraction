// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "EXOnlineBeaconClient.generated.h"

/**
 * 
 */
UCLASS()
class EX_API AEXOnlineBeaconClient : public AOnlineBeaconClient
{
	GENERATED_BODY()
	
public:


	//~ Begin AOnlineBeaconClient Interface
	virtual void OnFailure() override;
	//~ End AOnlineBeaconClient Interface

	/** Send a ping RPC to the client */
	UFUNCTION(Client, Reliable)
	virtual void ClientPing();

	/** Send a pong RPC to the host */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerPong();

	void Disconnect();

	bool Start(FString address, int32 Port = 7777, bool PortOverride = false);
};
