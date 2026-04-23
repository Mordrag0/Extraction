// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/EXLocalMessage.h"
#include "EXVoteMessage.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXVoteMessage : public UEXLocalMessage
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    /*  0 */ FText KickVote = NSLOCTEXT("UEXVoteMessage", "KickVote", "KickVote");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    /*  0 */ FText KickVoteOtherTeam = NSLOCTEXT("UEXVoteMessage", "KickVoteOtherTeam", "KickVoteOtherTeam");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    /*  0 */ FText KickVoteSpec = NSLOCTEXT("UEXVoteMessage", "KickVoteSpec", "KickVoteSpec");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    /*  1 */ FText ShuffleVote = NSLOCTEXT("UEXVoteMessage", "ShuffleVote", "ShuffleVote");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
    /*  1 */ FText ShuffleVoteSpec = NSLOCTEXT("UEXVoteMessage", "ShuffleVoteSpec", "ShuffleVoteSpec");


public:
	virtual FText GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = nullptr, class APlayerState* RelatedPlayerState_2 = nullptr, UObject* OptionalObject = nullptr) const override;

};
