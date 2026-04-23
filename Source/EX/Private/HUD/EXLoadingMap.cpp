// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EXLoadingMap.h"

void UEXLoadingMap::Show(const FString& Map)
{
	OnBeginMapLoad(Map);
}
