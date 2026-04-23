// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXOptions.generated.h"

class UEXLocalPlayer;
class FEXSettings;
class UButton;
class UCheckBox;

/** Delegate called when options are closed */
DECLARE_DELEGATE(FOnClosed);

/**
 * 
 */
UCLASS()
class EX_API UEXOptions : public UUserWidget
{
	GENERATED_BODY()
	

public:

	void Init(UEXLocalPlayer* LP);

	FOnClosed OnClosed;

protected:

	UFUNCTION()
	void Save();
	UFUNCTION()
	void Reset();
	UFUNCTION()
	void Close();

	TSharedPtr<FEXSettings> Settings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Options", Meta = (BindWidget))
	class UEXOptionsInput* InputOptions = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Options", Meta = (BindWidget))
	class UEXOptionsVideo* VideoOptions = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Options", Meta = (BindWidget))
	class UEXOptionsAudio* AudioOptions = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Options", Meta = (BindWidget))
	class UEXOptionsCrosshair* CrosshairOptions = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Options", Meta = (BindWidget))
	class UEXOptionsGeneral* GeneralOptions = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Options", Meta = (BindWidget))
	class UEXOptionsHUD* HUDOptions = nullptr;


	UPROPERTY(Meta = (BindWidget))
	UButton* SaveButton = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* ResetButton = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* CloseButton = nullptr;


};
