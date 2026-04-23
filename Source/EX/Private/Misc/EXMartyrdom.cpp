// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXMartyrdom.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXGrenade.h"

UEXMartyrdom::UEXMartyrdom()
{
	bUsedWhenDead = true;
}

// #EXTODO2
//void UEXMartyrdom::Server_Reliable_Fire_Implementation(uint8 Mode /*= 0*/, float WorldTimeOverride /**/)
//{
//	if (GrenadeClass)
//	{
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.Instigator = GetOwningCharacter();
//		AEXGrenade* Grenade = GetWorld()->SpawnActor<AEXGrenade>(GrenadeClass, GetOwningCharacter()->GetTransform(), SpawnParams);
//		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Explode, this, &UEXMartyrdom::Explode, Grenade->GetLifeTime());
//	}
//}
//
//bool UEXMartyrdom::Server_Reliable_Fire_Validate(uint8 Mode /*= 0*/, float WorldTimeOverride /**/)
//{
//	return true;
//}

void UEXMartyrdom::Explode()
{
	GetOwningCharacter()->SelfKill();
}

