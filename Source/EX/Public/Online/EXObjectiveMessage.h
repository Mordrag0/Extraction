// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/EXLocalMessage.h"
#include "EXObjectiveMessage.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXObjectiveMessage : public UEXLocalMessage
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  0 */ FText CarryableReturned = NSLOCTEXT("UEXObjectiveMessage", "CarryableReturned", "{Player1Name} returned the objective.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  1 */ FText CarryableTaken = NSLOCTEXT("UEXObjectiveMessage", "CarryableTaken", "{Player1Name} picked up the objective.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  2 */ FText CarryableDelivered = NSLOCTEXT("UEXObjectiveMessage", "CarryableDelivered", "{Player1Name} delivered the objective.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/* 12 */ FText CarryableTimedOut = NSLOCTEXT("UEXObjectiveMessage", "CarryableTimedOut", "Objective timed out.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  3 */ FText GenRepaired = NSLOCTEXT("UEXObjectiveMessage", "GenRepaired", "{Player1Name} repaired the generator.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  4 */ FText GenDestroyed = NSLOCTEXT("UEXObjectiveMessage", "GenDestroyed", "{Player1Name} destroyed the generator.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  5 */ FText C4Planted = NSLOCTEXT("UEXObjectiveMessage", "C4Planted", "{Player1Name} planted the C4.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  6 */ FText C4Defused = NSLOCTEXT("UEXObjectiveMessage", "C4Defused", "{Player1Name} defused the C4.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  7 */ FText FlagCaptureStarted = NSLOCTEXT("UEXObjectiveMessage", "FlagCaptureStarted", "{Player1Name} started capturing the flag.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  8 */ FText FlagCapturedAtt = NSLOCTEXT("UEXObjectiveMessage", "FlagCaptured", "Attackers captured the flag.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  13 */ FText FlagCapturedDef = NSLOCTEXT("UEXObjectiveMessage", "FlagCaptured", "Defenders captured the flag.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  9 */ FText EVRepaired = NSLOCTEXT("UEXObjectiveMessage", "EVRepaired", "{Player1Name} repaired the EV.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/* 10 */ FText EVDestroyed = NSLOCTEXT("UEXObjectiveMessage", "EVDestroyed", "{Player1Name} destroyed the EV.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/* 11 */ FText EVDestinationReached = NSLOCTEXT("UEXObjectiveMessage", "EVDestinationReached", "EV reached its destination.");

public:
	virtual FText GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = nullptr, class APlayerState* RelatedPlayerState_2 = nullptr, UObject* OptionalObject = nullptr) const override;

};
