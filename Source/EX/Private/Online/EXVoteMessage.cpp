// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/EXVoteMessage.h"
#include "Online/EXTeam.h"
#include "Online/EXPlayerState.h"

FText UEXVoteMessage::GetText(int32 Switch /*= 0*/, bool bTargetsPlayerState1 /*= false*/, class APlayerState* RelatedPlayerState_1 /*= nullptr*/, class APlayerState* RelatedPlayerState_2 /*= nullptr*/, UObject* OptionalObject /*= nullptr*/) const
{
	bool bSpec = true;
	bool bSameTeam = false;
	AEXPlayerState* PS = GetWorld()->GetFirstPlayerController()->GetPlayerState<AEXPlayerState>();
	AEXTeam* Team = PS ? PS->GetTeam() : nullptr;
	AEXPlayerState* VotedPS = Cast<AEXPlayerState>(RelatedPlayerState_1);
	AEXTeam* VotedTeam = VotedPS ? VotedPS->GetTeam() : nullptr;
	if (Team && VotedTeam)
	{
		if (!Team->IsSpectating())
		{
			bSpec = false;
			bSameTeam = VotedTeam->IsSame(Team);
		}
	}
	switch (Switch)
	{
		case 0: return (bSpec ? KickVoteSpec : (bSameTeam ? KickVote : KickVoteOtherTeam));
		case 1: return (bSpec ? ShuffleVoteSpec : ShuffleVote);
		default: return FText::GetEmpty();
	}
}
