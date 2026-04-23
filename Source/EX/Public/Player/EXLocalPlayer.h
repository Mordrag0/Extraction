// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "EXTypes.h"
#include "EXLocalPlayer.generated.h"

class UEXPersistentUser;
enum class ETeam : uint8;
enum class EChatDestination : uint8;

/**
 * 
 */
UCLASS(config = Engine, Transient)
class EX_API UEXLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	UEXLocalPlayer();

	virtual void SetControllerId(int32 NewControllerId) override;

	virtual FString GetNickname() const override;

	UEXPersistentUser* GetPersistentUser();

	/** Initializes the PersistentUser */
	void LoadPersistentUser();


	virtual void PostInitProperties() override;


	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;


	virtual void SaveChat(EChatDestination Destination, FString Sender, FString Message, FLinearColor Color, bool bMyChat, ETeam TeamType);

private:
	/** Persistent user data stored between sessions (i.e. the user's savegame) */
	UPROPERTY()
	UEXPersistentUser* PersistentUser = nullptr;

};
