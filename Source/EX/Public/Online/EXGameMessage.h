// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/EXLocalMessage.h"
#include "EXGameMessage.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXGameMessage : public UEXLocalMessage
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
	/*  0 */ FText RoundStart = NSLOCTEXT("UEXGameMessage", "RoundStart", "START!");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Message")
	/*  1 */ FText Overtime = NSLOCTEXT("UEXGameMessage", "Overtime", "OVERTIME");

public:
	virtual FText GetText(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const override;
	virtual FName GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const override;
	virtual FLinearColor GetMessageColor_Implementation(int32 MessageIndex) const override;
	virtual float GetScaleInSize_Implementation(int32 MessageIndex) const override;
	virtual int32 GetFontSizeIndex(int32 MessageIndex, bool bTargetsLocalPlayer) const override;
};
