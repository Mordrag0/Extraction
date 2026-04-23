// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXInGameMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class EX_API UEXInGameMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FORCEINLINE bool IsOpen() const { return bOpen; }

	UFUNCTION()
	void Open();
	UFUNCTION()
	void Close();

protected:
	UFUNCTION()
	void Save();
	UFUNCTION()
	void Reset();

	UFUNCTION()
	void CloseWithoutSave();
	UFUNCTION()
	void CloseWithSave();

	void LoadSettings();

	UFUNCTION()
	void Disconnect();

	bool bOpen = false;

	UPROPERTY(Meta = (BindWidget))
	UButton* BtnSave = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* BtnReset = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* BtnResume = nullptr;
	UPROPERTY(Meta = (BindWidget))
	UButton* BtnDisconnect = nullptr;



	virtual void NativeConstruct() override;

	bool bDirty = false;

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

};
