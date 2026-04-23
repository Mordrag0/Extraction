// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXChatStatusMessage.h"

UEXChatStatusMessage::UEXChatStatusMessage()
{
	Lifetime = -1.f; // Don't hide individual chat message rows
	bIsConsoleMessage = true;
}

FText UEXChatStatusMessage::GetText(int32 Switch /*= 0*/, bool bTargetsPlayerState1 /*= false*/, class APlayerState* RelatedPlayerState_1 /*= nullptr*/, class APlayerState* RelatedPlayerState_2 /*= nullptr*/, UObject* OptionalObject /*= nullptr*/) const
{
	switch (Switch)
	{
		case 0: return PlayerJoined;
		case 1: return PlayerLeft;
		default: return FText::GetEmpty();
	}
}
