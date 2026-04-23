// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/EXPersistentUser.h"
#include "EXOptionsCrosshair.generated.h"

class FEXSettings;
class UImage;
class UMaterialInstanceDynamic;
class UEditableTextBox;
class USlider;
class UTextBlock;
class UButton;
class UComboBoxString;

/**
 * 
 */
UCLASS()
class EX_API UEXOptionsCrosshair : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(TSharedPtr<FEXSettings> InSettings, bool bReset);

	TArray<FCrosshair> GetCrosshairs() { return Crosshairs; }
	int32 GetActiveCrosshairIdx() const { return ActiveCrosshairIdx; }
protected:
	TArray<FCrosshair> Crosshairs;
	int32 ActiveCrosshairIdx = 0;

	void LoadActiveCrosshair();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crosshair", Meta = (BindWidget))
	UImage* CrosshairImage = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	float CrosshairImageWidth = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	float CrosshairImageHeight = 500.f;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicCrosshair = nullptr;

	UFUNCTION()
	void PGapChanged(const FText& Text);
	UFUNCTION()
	void PSizeChanged(const FText& Text);
	UFUNCTION()
	void PThicknessChanged(const FText& Text);
	UFUNCTION()
	void PDynamicMultiplierChanged(const FText& Text);
	UFUNCTION()
	void DSizeChanged(const FText& Text);
	UFUNCTION()
	void CSizeChanged(const FText& Text);
	UFUNCTION()
	void CThicknessChanged(const FText& Text);
	UFUNCTION()
	void CDynamicMultiplierChanged(const FText& Text);
	UFUNCTION()
	void RedChanged(float Value);
	UFUNCTION()
	void GreenChanged(float Value);
	UFUNCTION()
	void BlueChanged(float Value);
	UFUNCTION()
	void AlphaChanged(float Value);
	UFUNCTION()
	void CrosshairAdded();
	UFUNCTION()
	void CrosshairRemoved();
	UFUNCTION()
	void CrosshairSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* PlusGap = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* PlusSize = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* PlusThickness = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* PlusDynamicMultiplier = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* DotSize = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* CircleSize = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* CircleThickness = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UEditableTextBox* CircleDynamicMultiplier = nullptr;
	UPROPERTY(Meta = (BindWidget))
	USlider* CrosshairRed = nullptr;
	UPROPERTY(Meta = (BindWidget))
	USlider* CrosshairGreen = nullptr;
	UPROPERTY(Meta = (BindWidget))
	USlider* CrosshairBlue = nullptr;
	UPROPERTY(Meta = (BindWidget))
	USlider* CrosshairAlpha = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* RedText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* GreenText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* BlueText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* AlphaText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* AddCrosshair = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* RemoveCrosshair = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UComboBoxString* CrosshairList = nullptr;

	TSharedPtr<FEXSettings> Settings;
};
