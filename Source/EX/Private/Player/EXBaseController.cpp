// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXBaseController.h"
#include "EX.h"
#include "Online/EXGameModeBase.h"
#include "Online/EXPlayerState.h"
#include "OnlineSubsystemTypes.h"
#include "Online/EXChatMessage.h"
#include "Online/EXGameStateSW.h"
#include "Online/EXGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "Net/UnrealNetwork.h"
#include "Player/EXPlayerController.h"
#include "Player/EXLocalPlayer.h"
#include "Player/EXPersistentUser.h"
#include "System/EXGameplayStatics.h"
#include "Misc/EXMusicManager.h"

UEXBaseChatWidget* AEXBaseController::GetChatWidget() const
{
	return nullptr;
}

void AEXBaseController::Say(FString Message)
{
	Say(Message, false, false);
}

void AEXBaseController::Say(FString Message, bool bTeam, bool bQuickMessage)
{
	if (Message.Len() == 0)
	{
		return;
	}

	// clamp message length; aside from troll prevention this is needed for networking reasons
	Message = Message.Left(MAX_CHAT_TEXT_SIZE);
	if (AllowTextMessage(Message, false))
	{
		Server_Reliable_Say(Message, bTeam, bQuickMessage);
	}
}

void AEXBaseController::InitPlayerState()
{
	Super::InitPlayerState();

	MyEXPS = Cast<AEXPlayerState>(PlayerState);
	if (MyEXPS)
	{
		MyEXPS->SetController(Cast<AEXPlayerController>(this));
	}
}

void AEXBaseController::OpenChat(bool bTeam)
{

}

void AEXBaseController::OnRep_PlayerState()
{
	MyEXPS = Cast<AEXPlayerState>(PlayerState);
	MyEXPS->SetController(Cast<AEXPlayerController>(this));
}

bool AEXBaseController::AllowTextMessage(FString& Msg, bool bIsTeamMessage)
{
	const float TIME_PER_MSG = 1.0f;
	const float MAX_OVERFLOW = 4.0f;

	if (IsNetMode(NM_Standalone))
	{
		return true;
	}

	if (HasAuthority())
	{
		// Give the game mode a chance to adjust/deny the message.
		AEXGameModeBase* BaseGameMode = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
		if (BaseGameMode && !BaseGameMode->AllowTextMessage(Msg, bIsTeamMessage, this))
		{
			return false;
		}

	}

	ChatOverflowTime = FMath::Max(ChatOverflowTime, GetWorld()->RealTimeSeconds);

	//When overflowed, wait till the time is back to 0
	if (bOverflowed && (ChatOverflowTime > GetWorld()->RealTimeSeconds))
	{
		return false;
	}
	bOverflowed = false;

	//Accumulate time for each message, double for a duplicate message
	ChatOverflowTime += TIME_PER_MSG;

	if (ChatOverflowTime - GetWorld()->RealTimeSeconds <= MAX_OVERFLOW)
	{
		return true;
	}

	bOverflowed = true;
	return false;
}

void AEXBaseController::DirectSay(const FString& Message)
{
	// Figure out who we are talking too
	AEXGameStateSW* GS = GetWorld()->GetGameState<AEXGameStateSW>();
	if (GS)
	{
		FString TargetPlayerName;
		FString FinalMessage = Message;

		bool bSent = false;

		// Look for a local player controller to send to
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AEXBaseController* PC = Cast<AEXBaseController>(*Iterator);
			if (PC && PC->PlayerState)
			{
				AEXPlayerState* TargetPlayerState = Cast<AEXPlayerState>(PC->PlayerState);
				if (TargetPlayerState)
				{
					TargetPlayerName = TargetPlayerState->GetPlayerName();

					if (Message.Left(TargetPlayerName.Len()).Equals(TargetPlayerName, ESearchCase::IgnoreCase))
					{
						FinalMessage = FinalMessage.Right(FinalMessage.Len() - TargetPlayerName.Len()).TrimStartAndEnd();
						bSent = true;

						TSharedPtr<const FUniqueNetId> Id = PC->PlayerState->GetUniqueId().GetUniqueNetId();
						bool bIsMuted = Id.IsValid() && IsPlayerMuted(Id.ToSharedRef().Get());

						if (!bIsMuted)
						{
							PC->Client_Reliable_Say(MyEXPS, FinalMessage, EChatDestination::Whisper);
						}
						FinalMessage = FString::Printf(TEXT("to %s \"%s\""), *TargetPlayerName, *FinalMessage);
						break;
					}
				}
			}
		}

		// If we haven't sent the message. Look to see if we need to forward this message elsewhere
		if (!bSent)
		{
			bSent = ForwardDirectSay(MyEXPS, FinalMessage);
		}

		if (bSent)
		{
			// Make sure I see that I sent it
			Client_Reliable_Say(MyEXPS, FinalMessage, EChatDestination::Whisper);
		}
	}
}

bool AEXBaseController::ForwardDirectSay(AEXPlayerState* SenderPlayerState, FString& FinalMessage)
{
	return false;
}

bool AEXBaseController::AllowSay(AEXPlayerState* Speaker, const FString& Message, EChatDestination Destination)
{
	return true;
	/*UUTProfileSettings* ProfileSettings = GetProfileSettings();
	if (ProfileSettings != nullptr && Speaker != nullptr)
	{
		if (ProfileSettings->ComFilter != EComFilter::AllComs)
		{
			if (ProfileSettings->ComFilter == EComFilter::NoComs)
			{
				return false;
			}
			else if (ProfileSettings->ComFilter == EComFilter::TeamComs)
			{
				if (Speaker->GetTeamNum() != GetTeamNum())
				{
					return false;
				}
			}
			else if (ProfileSettings->ComFilter == EComFilter::FriendComs)
			{
				if (!Speaker->bIsFriend)
				{
					return false;
				}
			}
		}
	}

	return !IsPlayerGameMuted(Speaker);*/
}

void AEXBaseController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenChat", IE_Pressed, this, &AEXBaseController::OpenChat<false>);
	InputComponent->BindAction("OpenTeamChat", IE_Pressed, this, &AEXBaseController::OpenChat<true>);
}

UEXPersistentUser* AEXBaseController::GetPersistentUser() const
{
	UEXLocalPlayer* const LocalPlayer = Cast<UEXLocalPlayer>(Player);
	return LocalPlayer ? LocalPlayer->GetPersistentUser() : nullptr;
}

void AEXBaseController::BeginPlay()
{
	Super::BeginPlay();
	UEXLocalPlayer* LP = Cast<UEXLocalPlayer>(GetLocalPlayer());
	UEXPersistentUser* PU = LP ? LP->GetPersistentUser() : nullptr;
	if (PU)
	{
		UEXGameplayStatics::SetMasterVolume(PU->GetMuted() ? 0.f : PU->GetMasterVolume());
		UEXGameplayStatics::SetMusicVolume(PU->GetMusicVolume()); 
		AEXMusicManager* MusicManager = UEXGameplayStatics::GetMusicManager(this);
		if (MusicManager)
		{
			// This will start playing music if volume is above 0
			MusicManager->RefreshVolume();
		}
	}
}

void AEXBaseController::Client_Reliable_Say_Implementation(AEXPlayerState* Speaker, const FString& Message, EChatDestination Destination)
{
	if (AllowSay(Speaker, Message, Destination))
	{
		FClientReceiveData ClientData;
		ClientData.LocalPC = this;
		ClientData.MessageIndex = (int32)Destination;
		ClientData.RelatedPlayerState_1 = Speaker;
		ClientData.MessageString = Message;

		UEXChatMessage::StaticClass()->GetDefaultObject<UEXChatMessage>()->ClientReceive(ClientData);
	}
}

void AEXBaseController::Server_Reliable_Say_Implementation(const FString& InMessage, bool bTeamMessage, bool bQuickChat)
{
	FString Message = InMessage;
	if (AllowTextMessage(Message, bTeamMessage) && PlayerState)
	{
		// Look to see if this message is a direct message to a given player.

		FString TrimmedMessage = Message.TrimStartAndEnd();
		if (TrimmedMessage.Left(1).Equals(TEXT("@")))
		{
			// Remove the @
			TrimmedMessage = TrimmedMessage.Right(TrimmedMessage.Len() - 1);
			DirectSay(TrimmedMessage);
		}
		else
		{
			bool bSpectatorMsg = PlayerState->IsOnlyASpectator();

			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				AEXBaseController* PC = Cast<AEXBaseController>(*Iterator);
				AEXPlayerState* PS = PC ? PC->GetPlayerState<AEXPlayerState>() : nullptr;
				if (PS)
				{
					if (!bTeamMessage || (PS->OnSameTeam(MyEXPS)))
					{
						bool bIsMuted = false;
						//TSharedPtr<const FUniqueNetId> Id = PlayerState->GetUniqueId().GetUniqueNetId();
						//bool bIsMuted = Id.IsValid()
						//	&& (PC->MuteList.VoiceMuteList.IndexOfByPredicate(FUniqueNetIdMatcher(*Id)) != INDEX_NONE);

						// Don't send spectator chat to players
						if ((!bSpectatorMsg || PS->IsOnlyASpectator()) && !bIsMuted)
						{
							EChatDestination Destination = bQuickChat
								? (bTeamMessage ? EChatDestination::TeamQuickChat : EChatDestination::LocalQuickChat)
								: (bTeamMessage ? EChatDestination::Team : EChatDestination::Local);
							PC->Client_Reliable_Say(MyEXPS, TrimmedMessage, Destination);
						}
					}
				}
			}
		}
	}
}

bool AEXBaseController::Server_Reliable_Say_Validate(const FString& InMessage, bool bTeamMessage, bool bQuickChat)
{
	return true;
}

bool AEXBaseController::EndSession()
{
	AEXPlayerState* PS = Cast<AEXPlayerState>(PlayerState);
	if (PS)
	{
		UE_LOG(LogEXOnline, Error, TEXT("Ending session on client *1"));
		IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
		if (OnlineSub)
		{
			UE_LOG(LogEXOnline, Error, TEXT("Ending session on client *2"));
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && (Sessions->GetNamedSession(PS->SessionName) != nullptr))
			{
				UE_LOG(LogEXOnline, Log, TEXT("Ending session %s on client"), *PS->SessionName.ToString());
				Sessions->EndSession(PS->SessionName);
				return true;
			}
		}
	}
	return false;
}

void AEXBaseController::LeaveOffileMap()
{
	UGameplayStatics::OpenLevel(this, FName("EntryMap"));
}


void AEXBaseController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}



void AEXBaseController::Mute()
{
	UEXPersistentUser* PersistentUser = GetPersistentUser();
	PersistentUser->SetMuted(true);
	PersistentUser->SaveIfDirty();
	UEXGameplayStatics::SetMasterVolume(0.f);

	AEXMusicManager* MM = UEXGameplayStatics::GetMusicManager(this);
	if (MM)
	{
		MM->RefreshVolume();
	}
}

void AEXBaseController::Unmute()
{
	UEXPersistentUser* PersistentUser = GetPersistentUser();
	PersistentUser->SetMuted(false);
	PersistentUser->SaveIfDirty();
	UEXGameplayStatics::SetMasterVolume(PersistentUser->GetMasterVolume());

	AEXMusicManager* MM = UEXGameplayStatics::GetMusicManager(this);
	if (MM)
	{
		MM->RefreshVolume();
	}
}


