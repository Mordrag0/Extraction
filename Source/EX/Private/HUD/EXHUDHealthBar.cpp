// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXHUDHealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Player/EXCharacter.h"
#include "Components/Image.h"
#include "Engine/World.h"

void UEXHUDHealthBar::SetCharacter(AEXCharacter* InCharacter)
{
	if (InCharacter)
	{
		MaxHealth = InCharacter->GetMaxHealth();
		MaxHealthInv = 1.f / InCharacter->GetMaxHealth();
		InCharacter->OnHealthChanged.AddDynamic(this, &UEXHUDHealthBar::UpdateHealthBar);
	}
}

void UEXHUDHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HealthBarMaterial) 
	{
		HealthBarMaterial->SetScalarParameterValue("CurrentTime", GetWorld()->GetRealTimeSeconds());
	}

}

void UEXHUDHealthBar::UpdateHealthBar(float Health, float HealthDelta, FDamageEvent const& DamageEvent, AController* InstigatedBy, AActor* DamageCauser)
{
	FString HealthString = (Health >= 0) ? FString(TEXT("+")) : FString(TEXT(""));
	
	HealthString.AppendInt((int32)Health);

	HealthText->SetText(FText::FromString(HealthString));

	if (!HealthBarMaterial) 
	{
		HealthBarMaterial = HealthBarImage->GetDynamicMaterial();
	}

	if (HealthBarMaterial)
	{
		HealthBarMaterial->SetScalarParameterValue("Percent", Health * MaxHealthInv);

		if (HealthDelta < 0)
		{
			float RangeValue = -HealthDelta / (MaxHealth - Health);
			HealthBarMaterial->SetScalarParameterValue("DamageRange", RangeValue);
		}
		else
		{
			HealthBarMaterial->SetScalarParameterValue("DamageRange", 0.f);
		}

		HealthBarMaterial->SetScalarParameterValue("TimeOfDamage", GetWorld()->GetRealTimeSeconds());
	}

	//Make the text flash red if low
	if (!HealthTextMaterial) 
	{
		HealthTextMaterial = HealthText->GetDynamicFontMaterial();
	}
	if (HealthTextMaterial)
	{
		if (Health * MaxHealthInv <= 0.25f) 
		{
			HealthTextMaterial->SetScalarParameterValue("Activate", 0.f);
		}
		else 
		{
			HealthTextMaterial->SetScalarParameterValue("Activate", 1.f);
		}
	}

}
