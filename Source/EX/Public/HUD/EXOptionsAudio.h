// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXOptionsAudio.generated.h"

class UEXPersistentUser;
class FEXSettings;

/**
 * 
 */
UCLASS()
class EX_API UEXOptionsAudio : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void Init(TSharedPtr<FEXSettings> InSettings, bool bReset);

	bool IsDirty() const { return bDirty; }
	void ClearDirty() {bDirty = false;}

protected:
	UFUNCTION()
	void MasterValueChanged(float Value);
	UFUNCTION()
	void MusicValueChanged(float Value);

	UPROPERTY(BlueprintReadOnly, Category = "Audio Options", Meta = (BindWidget))
	class USlider* MasterSlider = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Audio Options", Meta = (BindWidget))
	class UTextBlock* MasterVolumeText = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Audio Options", Meta = (BindWidget))
	class USlider* MusicSlider = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Audio Options", Meta = (BindWidget))
	class UTextBlock* MusicVolumeText = nullptr;

	float MasterVolume = 0.f;
	float MusicVolume = 0.f;

	TSharedPtr<FEXSettings> Settings = nullptr;

	bool bDirty = false;
};
