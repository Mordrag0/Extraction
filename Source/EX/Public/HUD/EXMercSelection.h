// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXMercSelection.generated.h"

class UEXMercIcon;
class AEXCharacter;

/**
 * 
 */
UCLASS()
class EX_API UEXMercSelection : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Show(UEXMercIcon* InSelectedIcon);
	UFUNCTION()
	void Apply(UEXMercIcon* Icon, FSquadMerc Merc);
protected:
	void SetSquad(const FSquad& Squad);

	UPROPERTY(Meta = (BindWidget))
	class UPanelWidget* MercPanel = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class UPanelWidget* MercPanel2 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEXMercIcon> MercIconClass = nullptr;

	UPROPERTY()
	UEXMercIcon* SelectedIcon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 NumIconsInFirstRow = 6;
};
 
