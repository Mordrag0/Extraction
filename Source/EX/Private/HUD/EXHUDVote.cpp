// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHUDVote.h"
#include "Online/EXVoteMessage.h"
#include "Components/TextBlock.h"
#include "Misc/EXInputInfo.h"
#include "System/EXGameplayStatics.h"

UEXHUDVote::UEXHUDVote(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BindInfoText = NSLOCTEXT("Vote", "Yes [{VoteYes}] / No [{VoteNo}]", "Yes [{VoteYes}] / No [{VoteNo}]");
}

void UEXHUDVote::StartVote(const FText& InText, int32 InDuration)
{
	bVoted = false;
	Text = InText;
	Duration = InDuration;
	TotalDuration = InDuration;
	Yeses = 1;
	SetVisibility(ESlateVisibility::HitTestInvisible);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RefreshVote, this, &UEXHUDVote::Refresh, 1.f, true, 0.f);

	UEXInputInfo* InputInfo = UEXGameplayStatics::GetInputInfoCDO();
	FFormatNamedArguments Args;
	Args.Add(TEXT("VoteYes"), FText::FromString(InputInfo->GetBindText(FName("VoteYes"))));
	Args.Add(TEXT("VoteNo"), FText::FromString(InputInfo->GetBindText(FName("VoteNo"))));
	FText BindText = FText::Format(BindInfoText, Args);
	BindInfo->SetText(BindText);
	BindInfo->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXHUDVote::UpdateVotes(int32 InYeses, int32 InNos)
{
	Yeses = InYeses;
	Nos = InNos;
}

void UEXHUDVote::Vote(bool bVote)
{
	if (bVoted)
	{
		return;
	}
	bVoted = true;
	BindInfo->SetVisibility(ESlateVisibility::Hidden);
}

void UEXHUDVote::EndVote()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UEXHUDVote::Refresh()
{
	if (Duration == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RefreshVote);
		SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	--Duration;

	FFormatNamedArguments Args;
	Args.Add(TEXT("YesVotes"), FText::FromString(FString::FromInt(Yeses)));
	Args.Add(TEXT("Duration"), FText::FromString(FString::FromInt(Duration)));
	FText DisplayText = FText::Format(Text, Args);
	Message->SetText(DisplayText);
}

