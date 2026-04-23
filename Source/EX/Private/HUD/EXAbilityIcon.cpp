// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXAbilityIcon.h"
#include "Inventory/EXInventory.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UEXAbilityIcon::UpdateInventory()
{
	if (Inventory)
	{
		const float CooldownTime = Inventory->GetCooldownTime();
		const float Uses = Inventory->GetUses();
		float IntPart;
		const float AbilityProgressVal = FMath::Modf(Uses, &IntPart);
		const int32 AbilityCountNum = FMath::RoundToInt(IntPart);

		if (UsesNumber && UsesIcon)
		{
			UsesNumber->SetText(FText::FromString(FString::FromInt(AbilityCountNum)));

			if (UsesIconMaterial)
			{
				UsesIconMaterial->SetScalarParameterValue("Percent", AbilityProgressVal <= 0.f ? 1.f : AbilityProgressVal);
			}
		}
		
		if (AbilityIcon && CooldownNumber)
		{
			if (AbilityCountNum == 0)
			{
				CooldownNumber->SetText(FText::FromString(FString::FromInt(CooldownTime - (CooldownTime * AbilityProgressVal))));
				CooldownNumber->SetVisibility(ESlateVisibility::HitTestInvisible);
				if (AbilityIconMaterial)
				{
					AbilityIconMaterial->SetScalarParameterValue("ProgressActive", 1.f);
					AbilityIconMaterial->SetScalarParameterValue("Percent", AbilityProgressVal);
				}
			}
			else
			{
				CooldownNumber->SetVisibility(ESlateVisibility::Hidden);
				if (AbilityIconMaterial)
				{
					AbilityIconMaterial->SetScalarParameterValue("ProgressActive", 0.f);
				}
			}
		}
	}
}

void UEXAbilityIcon::InitIcon(const UEXInventory* InInventory, UTexture* AbilityTexture)
{
	Inventory = InInventory;

	if (!AbilityIconMaterial)
	{
		AbilityIconMaterial = AbilityIcon->GetDynamicMaterial();
	}

	if (!UsesIconMaterial)
	{
		UsesIconMaterial = UsesIcon->GetDynamicMaterial();
	}

	if (AbilityTexture)
	{
		AbilityIconMaterial->SetTextureParameterValue("AbilityTexture", AbilityTexture);
	}
	UpdateInventory();
}

void UEXAbilityIcon::ShowIcon()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEXAbilityIcon::HideIcon()
{
	SetVisibility(ESlateVisibility::Hidden);
}

