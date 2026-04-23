// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EXTypes.h"
#include "EXBaseController.generated.h"

struct FClientReceiveData;
class UEXBaseChatWidget;
class AEXPlayerState;
class UEXHUDWidget;

/**
 * 
 */
UCLASS()
class EX_API AEXBaseController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual UEXHUDWidget* GetHUDWidget() const { return nullptr; }
	virtual UEXBaseChatWidget* GetChatWidget() const;
	UFUNCTION(exec)
	void Say(FString Message);
	void Say(FString Message, bool bTeam, bool bQuickMessage);


	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reliable_Say(const FString& InMessage, bool bTeamMessage, bool bQuickChat);

	AEXPlayerState* GetPS() const { return MyEXPS; }

	virtual void InitPlayerState() override;

	UFUNCTION()
	virtual void OpenChat(bool bTeam);

	template<bool bTeam>
	void OpenChat()
	{
		OpenChat(bTeam);
	}

	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable, Category = "Online")
	bool EndSession();

	void LeaveOffileMap();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region ConsoleCommands
	UFUNCTION(Exec)
	void Mute();
	UFUNCTION(Exec)
	void Unmute();
#pragma endregion ConsoleCommands
protected:
	void BeginPlay() override;

	UPROPERTY()
	AEXPlayerState* MyEXPS = nullptr;

	// Check to see if this PC can chat. Called on Client and server independently 
	bool AllowTextMessage(FString& Msg, bool bIsTeamMessage);

	// Sends a message directly to a user.  
	virtual void DirectSay(const FString& Message);

	// Forward the direct say to alternate servers
	virtual bool ForwardDirectSay(AEXPlayerState* SenderPlayerState, FString& FinalMessage);

	virtual bool AllowSay(AEXPlayerState* Speaker, const FString& Message, EChatDestination Destination);

	UFUNCTION(Client, Reliable)
	void Client_Reliable_Say(AEXPlayerState* Speaker, const FString& Message, EChatDestination Destination);

	// The accumulation of time added per message. Once overflowed the player must wait for this to return to 0 
	float ChatOverflowTime = 0.f;
	bool bOverflowed = false;

	void SetupInputComponent() override;

#pragma region ConsoleCommands
#pragma endregion ConsoleCommands

	UPROPERTY(EditDefaultsOnly, Category = "Controller")
	int32 MinNameLen = 2;
	UPROPERTY(EditDefaultsOnly, Category = "Controller")
	int32 MaxNameLen = 20;


	/** Returns the persistent user record associated with this player, or null if there isn't one. */
	class UEXPersistentUser* GetPersistentUser() const;


};
