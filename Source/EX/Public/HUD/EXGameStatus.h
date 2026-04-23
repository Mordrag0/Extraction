// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXMercStatusIcon.h"
#include "EXGameStatus.generated.h"

class UPanelWidget;
class AEXCharacter;
class AEXTeam;
class AEXPlayerState;
class UTextBlock;

/**
 * 
 */
UCLASS()
class EX_API UEXGameStatus : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Get the icon that belongs to the player
	UEXMercStatusIcon* GetIcon(AEXPlayerState* Player) const;

	// Assign an icon to a player
	void AssignIcon(AEXPlayerState* PS, TMap<UEXMercStatusIcon*, AEXPlayerState*>& Icons);
	// Unassign an icon from a players
	void FreeIcon(AEXPlayerState* PS);

	// Reassign icons
	UFUNCTION()
	void Refresh(AEXTeam* Attackers, AEXTeam* Defenders);

	// Need to know who the owner is so that owners team is on the left
	void SetOwner(AEXPlayerState* InOwner);

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* Friendly = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UPanelWidget* Enemy = nullptr;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TeamText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* EnemyText = nullptr;

	TMap<UEXMercStatusIcon*, AEXPlayerState*> FriendlyIcons;
	TMap<UEXMercStatusIcon*, AEXPlayerState*> EnemyIcons;

	AEXPlayerState* OwnerPS = nullptr;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText AttText = NSLOCTEXT("UEXGameStatus", "Att", "ATT");
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FText DefText = NSLOCTEXT("UEXGameStatus", "Def", "DEF");
};
