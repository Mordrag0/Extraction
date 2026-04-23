// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXSquad.generated.h"

class UEXMercIcon;

/**
 * 
 */
UCLASS()
class EX_API UEXSquad : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetSquad(const FSquad& Squad);

protected:
	UFUNCTION()
	void ChangeMerc(UEXMercIcon* Icon, FSquadMerc Merc);

	virtual void NativeConstruct() override;

	TArray<UEXMercIcon*> MercIcons;

	UPROPERTY(Meta = (BindWidget))
	class UPanelWidget* MercIconPanel = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	class UEXMercSelection* MercSelection = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	class UEXAugmentSelection* AugmentSelection = nullptr;
};
