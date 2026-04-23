// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXTypes.h"
#include "EXAugmentSelection.generated.h"

class UEXAugmentIcon;
class UEXMercIcon;

/**
 * 
 */
UCLASS()
class EX_API UEXAugmentSelection : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Show(UEXMercIcon* InMercIcon);

	UFUNCTION()
	void Apply();

protected:
	void Init(EMerc Merc);

	UFUNCTION()
	void AugmentClicked(UEXAugmentIcon* Icon, EAugment Augment);

	UPROPERTY(Meta = (BindWidget))
	class UPanelWidget* ButtonPanel = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Augment")
	TSubclassOf<UEXAugmentIcon> AugmentIconClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Augment")
	int32 MaxSelectedAugments = 3;

	TArray<EAugment> SelectedAugments;

	UEXMercIcon* MercIcon = nullptr;

	UPROPERTY(Meta = (BindWidget))
	class UButton* BtnApply = nullptr;

	virtual void NativeConstruct() override;

};
