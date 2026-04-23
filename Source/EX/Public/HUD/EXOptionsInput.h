// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerInput.h"
#include "../Player/EXPersistentUser.h"
#include "EXOptionsInput.generated.h"

class UCanvasPanel;
class UEXPersistentUser;
class UUniformGridPanel;
class UEXOptionsInputRow;
class FEXSettings;

/**
 * 
 */
UCLASS()
class EX_API UEXOptionsInput : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void Init(TSharedPtr<FEXSettings> InSettings, bool bReset);

public:
	UFUNCTION()
	void InputChanged(FMyInputId Id, FKey Key);
	UFUNCTION()
	void InvertYAxisChanged(bool Val);
	UFUNCTION()
	void MouseSensChanged(float Val);
	UFUNCTION()
	void ScopeSensScaleChanged(float Val);
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Options")
	TSubclassOf<UEXOptionsInputRow> RowClass = nullptr;
	TArray<UEXOptionsInputRow*> Rows;

	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* RowsPanel = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* InvertYAxis = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class USlider* MouseSensValue = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* MouseSensValueText = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class USlider* ScopeSensScaleValue = nullptr;
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* ScopeSensScaleValueText = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Options")
	float MinSensitivity = .01f;
	UPROPERTY(EditDefaultsOnly, Category = "Options")
	float MaxSensitivity = 10.f;

	bool bInvertYAxis = false;
	float MouseSens = 0.f;
	float ScopeSensitivityScale = 0.f;

	TSharedPtr<FEXSettings> Settings = nullptr;

	float SliderValueToSens(float Value);
	float SensToSliderValue(float Sens);
};
