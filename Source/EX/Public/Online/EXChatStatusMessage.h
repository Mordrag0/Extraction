// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/EXLocalMessage.h"
#include "EXChatStatusMessage.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXChatStatusMessage : public UEXLocalMessage
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
	/*  0 */ FText PlayerJoined = NSLOCTEXT("UEXChatMessage", "PlayerJoined", "{Player1Name} joined the game.");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
	/*  1 */ FText PlayerLeft = NSLOCTEXT("UEXChatMessage", "PlayerLeft", "{Player1Name} left the game.");

public:

	UEXChatStatusMessage();

	virtual FText GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = nullptr, class APlayerState* RelatedPlayerState_2 = nullptr, UObject* OptionalObject = nullptr) const override;

};
