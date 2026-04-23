// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Online/EXLocalMessage.h"
#include "EXAdminMessage.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXAdminMessage : public UEXLocalMessage
{
	GENERATED_BODY()

public:
	UEXAdminMessage();

	virtual FFormatNamedArguments GetArgs(int32 Switch, bool bTargetsPlayerState1, class APlayerState* RelatedPlayerState_1, class APlayerState* RelatedPlayerState_2, class UObject* OptionalObject) const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  0 */ FText AdminLoggedIn = NSLOCTEXT("UEXAdminMessage", "AdminLoggedIn", "Admin login successful.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  1 */ FText AdminBadPassword = NSLOCTEXT("UEXAdminMessage", "AdminBadPassword", "Wrong password.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  2 */ FText AdminAlreadyLoggedIn = NSLOCTEXT("UEXAdminMessage", "AdminAlreadyLoggedIn", "Already logged in as admin.");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  3 */ FText InvalidMapName = NSLOCTEXT("UEXAdminMessage", "InvalidMapName", "Invalid map name. Valid map names are: {Maps}");
	UPROPERTY(EditDefaultsOnly, Category = "Message")
	/*  4 */ FText NotAdmin = NSLOCTEXT("UEXAdminMessage", "NotAdmin", "You are not logged in as admin.");

public:
	virtual FText GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = nullptr, class APlayerState* RelatedPlayerState_2 = nullptr, UObject* OptionalObject = nullptr) const override;

};
