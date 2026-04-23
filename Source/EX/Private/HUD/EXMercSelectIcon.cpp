// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXMercSelectIcon.h"
#include "Components/Image.h"

void UEXMercSelectIcon::SetIcon(const TSoftObjectPtr<UTexture2D>& MercIcon)
{
	if (MercIcon.IsValid())
	{
		MercImage->SetBrushFromTexture(MercIcon.Get());
	}
}
