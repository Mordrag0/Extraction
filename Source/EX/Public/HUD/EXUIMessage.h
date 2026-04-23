// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXUIMessage.generated.h"

class UEXLocalMessage;
class APlayerState;
class UEXUIMessage;
class UEXBaseChatWidget;
class UEXMessageRow;
class UVerticalBox;
class UVerticalBoxSlot;

/**
 * 
 */
UCLASS()
class EX_API UEXUIMessage : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject);

	virtual void InitializeWidget(UEXBaseChatWidget* InHUDOwner);

	void Show();
	void ShowFor(float Duration);
	UFUNCTION()
	void Hide();

protected:

	float ShowDuration;

	FTimerHandle TimerHandle_Visible;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	FLinearColor RowBackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UEXMessageRow> RowClass;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	int32 RowCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	int32 OffsetPerRow = 40;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UEXBaseChatWidget* EXHUDOwner = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "HUD", Meta = (BindWidget))
	UVerticalBox* Panel = nullptr;

	UPROPERTY()
	TArray<UEXMessageRow*> Messages;

};
