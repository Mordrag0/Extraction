// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/EXBaseController.h"
#include "EXLobbyController.generated.h"

class UEXLobby;
class AEXGameModeLobby;

/**
 * 
 */
UCLASS()
class EX_API AEXLobbyController : public AEXBaseController
{
	GENERATED_BODY()

public:
	AEXLobbyController();

	UEXLobby* GetLobbyWidget() const { return Lobby; }

	UFUNCTION(Server, Reliable)
	void Server_Reliable_Vote(int32 Idx);

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	UEXLobby* Lobby = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TSubclassOf<UEXLobby> LobbyClass;

	UPROPERTY()
	AEXGameModeLobby* GM = nullptr;
};
