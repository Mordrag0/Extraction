// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXLocalMessage.h"
#include "EXChatMessage.generated.h"

struct FClientReceiveData;

/**
 * 
 */
UCLASS()
class EX_API UEXChatMessage : public UEXLocalMessage
{
	GENERATED_BODY()

public:
	UEXChatMessage();

	void ClientReceive(const FClientReceiveData& ClientData) const override;
};
