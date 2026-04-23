// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXGameStatus.h"
#include "HUD/EXMercStatusIcon.h"
#include "Online/EXTeam.h"
#include "Player/EXCharacter.h"
#include "Components/PanelWidget.h"
#include "Online/EXPlayerState.h"
#include "Online/EXLevelRules.h"
#include "System/EXGameplayStatics.h"
#include "Components/TextBlock.h"

UEXMercStatusIcon* UEXGameStatus::GetIcon(AEXPlayerState* PS) const
{
	const bool bFriendly = PS->OnSameTeam(OwnerPS);
	const TMap<UEXMercStatusIcon*, AEXPlayerState*>& Icons = bFriendly ? FriendlyIcons : EnemyIcons;
	UEXMercStatusIcon* const* Icon = Icons.FindKey(PS);
	if (!Icon)
	{
		return nullptr;
	}
	return *Icon;
}

void UEXGameStatus::AssignIcon(AEXPlayerState* PS, TMap<UEXMercStatusIcon*, AEXPlayerState*>& Icons)
{
	// Find a key where the value is null
	UEXMercStatusIcon* const* Icon = Icons.FindKey(nullptr);
	if (!Icon)
	{
		return;
	}
	// And assign it
	Icons[*Icon] = PS;
	(*Icon)->SetVisibility(ESlateVisibility::HitTestInvisible);

	// Set the icon
	AEXCharacter* Char = PS->GetPawn<AEXCharacter>();
	if (Char)
	{
		Char->SetStatusIcon(GetIcon(PS));
	}
}

void UEXGameStatus::FreeIcon(AEXPlayerState* PS)
{
	const bool bFriendly = PS->OnSameTeam(OwnerPS);
	TMap<UEXMercStatusIcon*, AEXPlayerState*>& Icons = bFriendly ? FriendlyIcons : EnemyIcons;
	UEXMercStatusIcon* const* Icon = Icons.FindKey(PS);
	if (!*Icon)
	{
		return;
	}
	Icons[*Icon] = nullptr;
	(*Icon)->SetVisibility(ESlateVisibility::Collapsed);
}

void UEXGameStatus::Refresh(AEXTeam* Attackers, AEXTeam* Defenders)
{
	for (TTuple<UEXMercStatusIcon*, AEXPlayerState*>& Pair : FriendlyIcons)
	{
		Pair.Key->SetDefaultTexture();
		Pair.Key->SetVisibility(ESlateVisibility::Collapsed);
		Pair.Value = nullptr;
	}
	for (TTuple<UEXMercStatusIcon*, AEXPlayerState*>& Pair : EnemyIcons)
	{
		Pair.Key->SetDefaultTexture();
		Pair.Key->SetVisibility(ESlateVisibility::Collapsed);
		Pair.Value = nullptr;
	}

	const bool bOwnerDefending = OwnerPS->GetTeam()->IsDefending();
	for (AEXPlayerState* PS : Attackers->GetMembers())
	{
		AssignIcon(PS, !bOwnerDefending ? FriendlyIcons : EnemyIcons);
	}
	for (AEXPlayerState* PS : Defenders->GetMembers())
	{
		AssignIcon(PS, bOwnerDefending ? FriendlyIcons : EnemyIcons);
	}
	UE_LOG(LogTemp, Warning, TEXT("%d, %d"), Attackers->GetMembers().Num(), Defenders->GetMembers().Num());

	// Set the team text
	TeamText->SetText(bOwnerDefending ? DefText : AttText);
	EnemyText->SetText(bOwnerDefending ? AttText : DefText);
}

void UEXGameStatus::SetOwner(AEXPlayerState* InOwner)
{
	OwnerPS = InOwner;
}

// Get Icons from both panels and save them
void UEXGameStatus::NativeConstruct()
{
	Super::NativeConstruct();
	FriendlyIcons.Empty();
	EnemyIcons.Empty();

	TArray<UEXMercStatusIcon*> FriendlyIconArr = UEXGameplayStatics::GetPanelChildren<UEXMercStatusIcon>(Friendly, true);
	Algo::Reverse(FriendlyIconArr);
	for (UEXMercStatusIcon* Icon : FriendlyIconArr)
	{
		Icon->SetPlayer(nullptr);
		Icon->SetStatus(EMercStatus::None);
		Icon->SetTeamColor(true);
		FriendlyIcons.Add(Icon, nullptr);
	}
	TArray<UEXMercStatusIcon*> EnemyIconArr = UEXGameplayStatics::GetPanelChildren<UEXMercStatusIcon>(Enemy, true);
	for (UEXMercStatusIcon* Icon : EnemyIconArr)
	{
		Icon->SetPlayer(nullptr);
		Icon->SetStatus(EMercStatus::None);
		Icon->SetTeamColor(true);
		EnemyIcons.Add(Icon, nullptr);
	}
}

