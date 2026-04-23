// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXChatMessage.h"
#include "EXTypes.h"
#include "HUD/EXGameViewportClient.h"
#include "GameFramework/LocalMessage.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"
#include "EX.h"
#include "Player/EXLocalPlayer.h"
#include "Player/EXBaseController.h"
#include "Engine/Console.h"
#include "HUD/EXHUDWidget.h"


UEXChatMessage::UEXChatMessage()
{
	Lifetime = -1.f; // Don't hide individual chat message rows
	bIsConsoleMessage = true;
}

void UEXChatMessage::ClientReceive(const FClientReceiveData& ClientData) const
{
	// #ChatWidget

	AEXBaseController* PC = Cast<AEXBaseController>(ClientData.LocalPC);
	UEXHUDWidget* HUD = PC ? PC->GetHUDWidget() : nullptr;
	if (HUD)
	{
		AEXPlayerState* PS = Cast<AEXPlayerState>(ClientData.RelatedPlayerState_1);
		AEXTeam* Team = PS ? PS->GetTeam() : nullptr;
		ETeam TeamType = Team ? Team->GetType() : ETeam::Spectator;
		FString PlayerName = PS ? PS->GetPlayerName() : TEXT("Player");

		FText LocalMessageText = FText::FromString(FString::Printf(TEXT("%s: %s"), *PlayerName, *ClientData.MessageString));
		HUD->ReceiveLocalMessage(GetClass(), ClientData.RelatedPlayerState_1, ClientData.RelatedPlayerState_2, ClientData.MessageIndex, LocalMessageText, ClientData.OptionalObject);

		// Pipe console messages to the demo playback hud for live view of chat
		UEXLocalPlayer* LP = Cast<UEXLocalPlayer>(ClientData.LocalPC->Player);
		UEXGameViewportClient* GVC = LP ? Cast<UEXGameViewportClient>(LP->ViewportClient) : nullptr;
		if (GVC && GVC->HasActiveWorldOverride())
		{
			UWorld* DemoWorld = GVC->GetWorld();
			if (DemoWorld)
			{
				for (FConstPlayerControllerIterator Iterator = DemoWorld->GetPlayerControllerIterator(); Iterator; ++Iterator)
				{
					if (Iterator->Get()->Player == LP)
					{
						/*AUTHUD* DemoHUD = Cast<AUTHUD>(Iterator->Get()->MyHUD);
						if (DemoHUD)
						{
							DemoHUD->ReceiveLocalMessage(GetClass(), ClientData.RelatedPlayerState_1, ClientData.RelatedPlayerState_2, MessageIndex, LocalMessageText, ClientData.OptionalObject);
						}*/
					}
				}
			}
		}

		TArray<FStringFormatArg> StringArgs;
		StringArgs.Add(FStringFormatArg(UEnum::GetValueAsString((EChatDestination)ClientData.MessageIndex)));
		FString DestinationTag = FString::Format(TEXT("[{0}]"), StringArgs);
		FString ChatMessage;
		EChatDestination Destination = (EChatDestination)ClientData.MessageIndex;
		if (Destination == EChatDestination::MOTD || Destination == EChatDestination::System || Destination == EChatDestination::Lobby)
		{
			ChatMessage = FString::Printf(TEXT("%s %s"), *DestinationTag, *ClientData.MessageString);
		}
		else
		{
			ChatMessage = FString::Printf(TEXT("%s %s: %s"), *DestinationTag, *PlayerName, *ClientData.MessageString);
		}

		if (IsConsoleMessage(ClientData.MessageIndex) && LP && LP->ViewportClient)
		{
			LP->ViewportClient->ViewportConsole->OutputText(ChatMessage);
		}

		if (PC && LP)
		{
			FLinearColor ChatColor = ((ClientData.MessageIndex != (int32)EChatDestination::None) && Team) ? Team->GetColor() : FLinearColor::White;
			LP->SaveChat(Destination, PlayerName, ClientData.MessageString, ChatColor, (ClientData.RelatedPlayerState_1 == PC->PlayerState), TeamType);
		}
	}
}
