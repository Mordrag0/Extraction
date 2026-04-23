// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EXProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class EX_API UEXProgressWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Category = "Progressbar")
	class UProgressBar* ProgressBar = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Progressbar")
	class UEXProgress* ProgressComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Progressbar")
	FLinearColor Color = FLinearColor::Yellow;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateProgress(float Progress);

public:
	UFUNCTION(BlueprintCallable, Category = "Progressbar")
	void SetProgressComponent(UEXProgress* Progress, bool bInReversed = false);

	UFUNCTION(BlueprintCallable, Category = "Progressbar")
	void SetColor(FLinearColor InColor);

protected:
	bool bReversed = false;
	bool bOldVisibility;
};
