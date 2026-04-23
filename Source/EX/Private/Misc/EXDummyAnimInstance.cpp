// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXDummyAnimInstance.h"

void UEXDummyAnimInstance::OnHit_Implementation()
{

}

void UEXDummyAnimInstance::OnDead_Implementation()
{
	bDead = true;
}

void UEXDummyAnimInstance::Reset()
{
	bDead = false;
}
