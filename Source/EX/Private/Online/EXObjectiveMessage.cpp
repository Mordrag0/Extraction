// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXObjectiveMessage.h"

FText UEXObjectiveMessage::GetText(int32 Switch /*= 0*/, bool bTargetsPlayerState1 /*= false*/, class APlayerState* RelatedPlayerState_1 /*= nullptr*/, class APlayerState* RelatedPlayerState_2 /*= nullptr*/, UObject* OptionalObject /*= nullptr*/) const
{
	switch(Switch)
	{
		case 0: return CarryableReturned;
		case 1: return CarryableTaken;
		case 2: return CarryableDelivered;
		case 3: return GenRepaired;
		case 4: return GenDestroyed;
		case 5: return C4Planted;
		case 6: return C4Defused;
		case 7: return FlagCaptureStarted;
		case 8: return FlagCapturedAtt;
		case 9: return EVRepaired;
		case 10: return	EVDestroyed;
		case 11: return	EVDestinationReached;
		case 12: return	CarryableTimedOut;
		case 13: return	FlagCapturedDef;
		default: return FText::GetEmpty();
	}
}
