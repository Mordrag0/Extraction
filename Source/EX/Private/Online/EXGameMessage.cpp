// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXGameMessage.h"
#include "GameFramework/LocalMessage.h"


int32 UEXGameMessage::GetFontSizeIndex(int32 MessageIndex, bool bTargetsLocalPlayer) const
{
	return 1;
}

float UEXGameMessage::GetScaleInSize_Implementation(int32 MessageIndex) const
{
	return 3.f;
}

FLinearColor UEXGameMessage::GetMessageColor_Implementation(int32 MessageIndex) const
{
	return FLinearColor::White;
}

FText UEXGameMessage::GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const
{
	switch (Switch)
	{
		case 0: return RoundStart;
		case 1: return Overtime;
		default: return FText::GetEmpty();
	}
}

FName UEXGameMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const
{
	switch (Switch)
	{
		case 1: return TEXT("Overtime"); break;
		case 9: return TEXT("YouAreOnRedTeam"); break;
		case 10: return TEXT("YouAreOnBlueTeam"); break;
		case 16: return TEXT("TheMatchIsStarting"); break;
	}
	return NAME_None;
}
