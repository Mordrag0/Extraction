// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXGameResultTeamScore.generated.h"

class UVerticalBox;

/**
 * 
 */
UCLASS()
class EX_API UEXGameResultTeamScore : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(const FRoundScore& Score);

	UPROPERTY(Meta = (BindWidget))
	UVerticalBox* Root = nullptr;
};
