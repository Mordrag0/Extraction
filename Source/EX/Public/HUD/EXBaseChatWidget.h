// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXBaseChatWidget.generated.h"

class UEXLocalMessage;
class UEXUIMessage;
class UEXUIChatMessage;
class UEXInputBox;
class UEXHUDWidget;

/**
 * 
 */
UCLASS()
class EX_API UEXBaseChatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, int32 MessageIndex, FText LocalMessageText, UObject* OptionalObject);

	FORCEINLINE bool IsTyping() const { return bTyping; }

	void SetHUD(UEXHUDWidget* InHUD);

	void StartTyping(bool bTeam);

	void StopTyping(bool bCancel);

protected:
	bool bTyping = false;

	void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message", Meta = (BindWidget))
	UEXUIMessage* ChatBox = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Message", Meta = (BindWidget))
	UEXInputBox* InputBox = nullptr;

	UFUNCTION()
	virtual void Submit(const FText& Text, ETextCommit::Type CommitMethod);

	class UEXHUDWidget* HUD = nullptr;

	UPROPERTY()
	float Duration = 5.f;
};
