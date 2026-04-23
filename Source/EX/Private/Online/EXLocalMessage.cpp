// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXLocalMessage.h"
#include "Engine/DemoNetDriver.h"
#include "Player/EXPlayerController.h"
#include "Engine/Console.h"
#include "HUD/EXHUDWidget.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"
#include "Inventory/EXDamageType.h"

void UEXLocalMessage::ClientReceive(const FClientReceiveData& ClientData) const
{
	// #Messages
	// Skip playing local messages if we're fast forwarding from a demo seek
	UDemoNetDriver* DemoDriver = ClientData.LocalPC->GetWorld()->GetDemoNetDriver();
	if (DemoDriver)
	{
		if (DemoDriver->IsFastForwarding())
		{
			return;
		}
	}

	FText LocalMessageText = ResolveMessage(ClientData.MessageIndex, (ClientData.RelatedPlayerState_1 == ClientData.LocalPC->PlayerState), ClientData.RelatedPlayerState_1, ClientData.RelatedPlayerState_2, ClientData.OptionalObject);
	if (!LocalMessageText.IsEmpty())
	{
		// #ChatWidget
		AEXPlayerController* PC = Cast<AEXPlayerController>(ClientData.LocalPC);
		UEXHUDWidget* HUD = PC ? PC->GetHUDWidget() : nullptr;
		if (HUD)
		{
			HUD->ReceiveLocalMessage(GetClass(), ClientData.RelatedPlayerState_1, ClientData.RelatedPlayerState_2, ClientData.MessageIndex, LocalMessageText, ClientData.OptionalObject);
		}

		if (IsConsoleMessage(ClientData.MessageIndex))
		{
			ULocalPlayer* LP = Cast<ULocalPlayer>(ClientData.LocalPC->Player);
			UGameViewportClient* GVC = LP ? LP->ViewportClient : nullptr;
			if (GVC) 
			{
				GVC->ViewportConsole->OutputText(GetConsoleString(ClientData, LocalMessageText));
			}
		}
	}

	OnClientReceive(ClientData.LocalPC, ClientData.MessageIndex, ClientData.RelatedPlayerState_1, ClientData.RelatedPlayerState_2, ClientData.OptionalObject);
}

FString UEXLocalMessage::GetConsoleString(const FClientReceiveData& ClientData, FText LocalMessageText) const
{
	return LocalMessageText.ToString();
}

bool UEXLocalMessage::IsConsoleMessage(int32 Switch) const
{
	return GetDefault<UEXLocalMessage>(GetClass())->bIsConsoleMessage;
}

FFormatNamedArguments UEXLocalMessage::GetArgs(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
	FFormatNamedArguments Args;
	AEXPlayerState* PS1 = Cast<AEXPlayerState>(RelatedPlayerState_1);
	AEXPlayerState* PS2 = Cast<AEXPlayerState>(RelatedPlayerState_1);

	Args.Add(TEXT("Player1Name"), RelatedPlayerState_1 ? FText::FromString(RelatedPlayerState_1->GetPlayerName()) : FText::GetEmpty());
	Args.Add(TEXT("Player1Score"), PS1 ? FText::AsNumber(PS1->GetPlayerScore()) : FText::GetEmpty());
	Args.Add(TEXT("Player2Name"), RelatedPlayerState_2 ? FText::FromString(RelatedPlayerState_2->GetPlayerName()) : FText::GetEmpty());
	Args.Add(TEXT("Player2Score"), PS2 ? FText::AsNumber(PS2->GetPlayerScore()) : FText::GetEmpty());
	Args.Add(TEXT("Player1OldName"), RelatedPlayerState_1 ? FText::FromString(RelatedPlayerState_1->GetOldPlayerName()) : FText::GetEmpty());

	AEXTeam* Team1 = PS1 ? PS1->GetTeam() : nullptr;
	Args.Add(TEXT("Player1Team"), Team1 ? FText::FromString(Team1->GetTeamName()) : FText::GetEmpty());

	AEXTeam* Team2 = PS2 ? PS2->GetTeam() : nullptr;
	Args.Add(TEXT("Player1Team"), Team2 ? FText::FromString(Team2->GetTeamName()) : FText::GetEmpty());


	UClass* DamageTypeClass = Cast<UClass>(OptionalObject);
	if (DamageTypeClass && DamageTypeClass->IsChildOf(UEXDamageType::StaticClass()))
	{
		UEXDamageType* DamageType = DamageTypeClass->GetDefaultObject<UEXDamageType>();
		if (DamageType)
		{
			Args.Add(TEXT("WeaponName"), DamageType->AssociatedWeaponName);
		}
	}

	return Args;
}

FText UEXLocalMessage::GetText(int32 Switch /*= 0*/, bool bTargetsPlayerState1 /*= false*/, class APlayerState* RelatedPlayerState_1 /*= nullptr*/, class APlayerState* RelatedPlayerState_2 /*= nullptr*/, UObject* OptionalObject /*= nullptr*/) const
{
	return Blueprint_GetText(Switch, bTargetsPlayerState1, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject);
}

float UEXLocalMessage::GetScaleInSize_Implementation(int32 MessageIndex) const
{
	return ScaleInSize;
}

float UEXLocalMessage::GetLifeTime(int32 Switch) const
{
	return Blueprint_GetLifeTime(Switch);
}

float UEXLocalMessage::Blueprint_GetLifeTime_Implementation(int32 Switch) const
{
	return GetDefault<UEXLocalMessage>(GetClass())->Lifetime;
}

int32 UEXLocalMessage::GetFontSizeIndex(int32 MessageIndex, bool bTargetsLocalPlayer) const
{
	return FontSizeIndex;
}

FName UEXLocalMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const
{
	return NAME_None;
}

USoundBase* UEXLocalMessage::GetAnnouncementSound_Implementation(int32 Switch, const UObject* OptionalObject) const
{
	return nullptr;
}

FText UEXLocalMessage::Blueprint_GetText_Implementation(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
	return FText::GetEmpty();
}

FLinearColor UEXLocalMessage::GetMessageColor_Implementation(int32 MessageIndex) const
{
	return FLinearColor::White;
}

FText UEXLocalMessage::ResolveMessage_Implementation(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
	const FFormatNamedArguments Args = GetArgs(Switch, bTargetsPlayerState1, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject);
	return FText::Format(GetText(Switch, bTargetsPlayerState1, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject), Args);
}
