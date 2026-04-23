// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXMercStatusIcon.generated.h"

class UImage;
class UTexture2D;
class AEXPlayerState;

/**
 * 
 */
UCLASS()
class EX_API UEXMercStatusIcon : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetDefaultTexture();
	void SetTexture(TSoftObjectPtr<UTexture2D> CharacterIcon);
	void SetStatus(EMercStatus Status);
	void SetTeamColor(bool bFriendly);

	UPROPERTY(EditDefaultsOnly, Meta = (BindWidget))
	UImage* Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, Meta = (BindWidget))
	UImage* DeathEffect = nullptr;
	UPROPERTY(EditDefaultsOnly, Meta = (BindWidget))
	UImage* GibbedEffect = nullptr;

	AEXPlayerState* GetPlayer() const { return Player; }
	void SetPlayer(AEXPlayerState* InPlayer) { Player = InPlayer; }
protected:
	AEXPlayerState* Player = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UTexture2D* DefaultTexture = nullptr;
};
