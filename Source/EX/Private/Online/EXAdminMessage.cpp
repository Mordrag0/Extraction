// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXAdminMessage.h"
#include "EX.h"

UEXAdminMessage::UEXAdminMessage()
{
	bIsConsoleMessage = true;
}

FFormatNamedArguments UEXAdminMessage::GetArgs(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const
{
	FFormatNamedArguments Args = Super::GetArgs(Switch, bTargetsPlayerState1, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject); 

	FString Maps;
	for (int32 MapIdx = 0; MapIdx < ValidMapNames.Num(); MapIdx++)
	{
		Maps += ValidMapNames[MapIdx].ToString();
		if (MapIdx < ValidMapNames.Num()-1)
		{
			Maps += FString(", ");
		}
	}

	Args.Add(TEXT("Maps"), FText::FromString(Maps));
	return Args;
}

FText UEXAdminMessage::GetText(int32 Switch /*= 0*/, bool bTargetsPlayerState1 /*= false*/, class APlayerState* RelatedPlayerState_1 /*= nullptr*/, class APlayerState* RelatedPlayerState_2 /*= nullptr*/, UObject* OptionalObject /*= nullptr*/) const
{
	switch (Switch)
	{
		case 0: return AdminLoggedIn;
		case 1: return AdminBadPassword;
		case 2: return AdminAlreadyLoggedIn;
		case 3: return InvalidMapName;
		case 4: return NotAdmin;
		default: return FText::GetEmpty();
	}
}
