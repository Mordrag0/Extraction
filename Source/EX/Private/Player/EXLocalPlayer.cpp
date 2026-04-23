// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXLocalPlayer.h"
#include "Online/EXGameInstance.h"
#include "Player/EXPersistentUser.h"
#include "OnlineSubsystemUtilsClasses.h"
#include "EX.h"
#include "Online.h"
#include "System/EXGameplayStatics.h"

UEXLocalPlayer::UEXLocalPlayer()
{
}

void UEXLocalPlayer::SetControllerId(int32 NewControllerId)
{
	FString SaveGameName = GetNickname();

	ULocalPlayer::SetControllerId(NewControllerId);

	// If we changed controllerid / user, then we need to load the appropriate persistent user.
	if (PersistentUser != nullptr && (GetControllerId() != PersistentUser->GetUserIndex() || SaveGameName != PersistentUser->GetName()))
	{
		PersistentUser->SaveIfDirty();
		PersistentUser = nullptr;
	}

	if (!PersistentUser)
	{
		LoadPersistentUser();
	}
}

FString UEXLocalPlayer::GetNickname() const
{
	FString UserNickName = Super::GetNickname();

	if (UserNickName.Len() > MAX_PLAYER_NAME_LENGTH)
	{
		UserNickName = UserNickName.Left(MAX_PLAYER_NAME_LENGTH) + "...";
	}

	bool bReplace = (UserNickName.Len() == 0);

	// Check for duplicate nicknames...and prevent reentry
	static bool bReentry = false;
	if (!bReentry)
	{
		bReentry = true;
		UEXGameInstance* GameInstance = GetWorld() ? Cast<UEXGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
		if (GameInstance)
		{
			// Check all the names that occur before ours that are the same
			const TArray<ULocalPlayer*>& LocalPlayers = GameInstance->GetLocalPlayers();
			for (int Idx = 0; Idx < LocalPlayers.Num(); ++Idx)
			{
				const ULocalPlayer* LocalPlayer = LocalPlayers[Idx];
				if (LocalPlayer == this)
				{
					break;
				}

				if (UserNickName == LocalPlayer->GetNickname())
				{
					bReplace = true;
					break;
				}
			}
		}
		bReentry = false;
	}

	if (bReplace)
	{
		UserNickName = FString::Printf(TEXT("Player%i"), GetControllerId() + 1);
	}

	return UserNickName;
}

UEXPersistentUser* UEXLocalPlayer::GetPersistentUser()
{
	// If persistent data isn't loaded yet, load it
	if (!PersistentUser)
	{
		LoadPersistentUser();
	}
	return PersistentUser;
}

void UEXLocalPlayer::LoadPersistentUser()
{
	FString SaveGameName = GetNickname();

	// If we changed controllerid / user, then we need to load the appropriate persistent user.
	if (PersistentUser && ((GetControllerId() != PersistentUser->GetUserIndex()) || (SaveGameName != PersistentUser->GetName())))
	{
		PersistentUser->SaveIfDirty();
		PersistentUser = nullptr;
	}

	if (!PersistentUser)
	{
		// Use the platform id here to be resilient in the face of controller swapping and similar situations.
		FPlatformUserId PlatformId = FPlatformMisc::GetPlatformUserForUserIndex(GetControllerId());

		auto Identity = Online::GetIdentityInterface();
		if (Identity.IsValid() && GetPreferredUniqueNetId().IsValid())
		{
			PlatformId = Identity->GetPlatformUserIdFromUniqueNetId(*GetPreferredUniqueNetId());
		}

		PersistentUser = UEXPersistentUser::LoadPersistentUser(SaveGameName, 0);
	}
}

void UEXLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (!PersistentUser)
	{
		LoadPersistentUser();
	}
}

bool UEXLocalPlayer::Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return Super::Exec(InWorld, Cmd, Ar);
}

void UEXLocalPlayer::SaveChat(EChatDestination Destination, FString Sender, FString Message, FLinearColor Color, bool bMyChat, ETeam TeamType)
{

}
