// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "Components/TextBlock.h"
#include "EXMessageRow.generated.h"

class UTextBlock;
class UBorder;

/**
 * 
 */
UCLASS()
class EX_API UEXMessageRow : public UEXHUDElement
{
	GENERATED_BODY()
	
public:

	virtual void ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, uint32 MessageIndex, FText LocalMessageText, UObject* OptionalObject);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetMessage(FText Message);

	void CopyFrom(UEXMessageRow* OtherRow);

	FText GetText() const { return RowText->GetText(); }

	void SetBGColor(const FLinearColor& InColor);

	UFUNCTION()
	void Hide();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Message", Meta = (BindWidget))
	UBorder* RowBG = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Message", Meta = (BindWidget))
	UTextBlock* RowText = nullptr;


private:

	FTimerHandle TimerHandle_Visible;
};
