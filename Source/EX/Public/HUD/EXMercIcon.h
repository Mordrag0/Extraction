// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXMercIcon.generated.h"

class UEXAugmentIcon;
class UEXAugmentSelection;
class UEXMercSelection;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMercIconClicked, UEXMercIcon*, Icon, FSquadMerc, Merc);

/**
 * 
 */
UCLASS()
class EX_API UEXMercIcon : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void Init(UEXAugmentSelection* InAugmentSelection, const FSquadMerc& InMerc);
	EMerc GetMercType() const { return Merc; }
	FOnMercIconClicked OnIconClicked;
	void HideAugments();
protected:

	virtual void NativeConstruct() override;
	void OnValueChanged();

	// Merc
	EMerc Merc = EMerc::Max;

	UPROPERTY(Meta = (BindWidget))
	class UButton* MercIconBtn = nullptr;
	UFUNCTION()
	void MercIconClicked();

	// Augments
	TArray<EAugment> GetAugmentValues() const;

	UPROPERTY()
	UEXAugmentSelection* AugmentSelection = nullptr;

	TArray<UEXAugmentIcon*> AugmentIcons;

	UPROPERTY(Meta = (BindWidget))
	class UPanelWidget* AugmentPanel = nullptr;
	UFUNCTION()
	void OnAugmentClicked(UEXAugmentIcon* Icon, EAugment Augment);

	UPROPERTY(Meta = (BindWidget))
	class UImage* MercIcon = nullptr;
};
