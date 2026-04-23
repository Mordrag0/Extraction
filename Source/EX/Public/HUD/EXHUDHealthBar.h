// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXHUDElement.h"
#include "EXHUDHealthBar.generated.h"


class AEXCharacter;
class UMaterialInstanceDynamic;

/**
 * 
 */
UCLASS()
class EX_API UEXHUDHealthBar : public UEXHUDElement
{
	GENERATED_BODY()
	
public:

	void SetCharacter(AEXCharacter* InCharacter);

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void UpdateHealthBar(float Health, float HealthDelta, struct FDamageEvent const& DamageEvent, class AController* InstigatedBy, class AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "HealthBar", Meta = (BindWidget))
	class UImage* HealthBarImage = nullptr;
		
	UPROPERTY(BlueprintReadOnly, Category = "HealthBar", Meta = (BindWidget))
	class UTextBlock* HealthText = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* HealthTextMaterial = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* HealthBarMaterial = nullptr;

	float MaxHealth = 1.f;
	float MaxHealthInv = 1.f;
};
