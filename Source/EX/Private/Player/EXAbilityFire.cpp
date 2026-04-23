// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXAbilityFire.h"

FString UEXAbilityFire::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

#if WITH_EDITOR
void UEXAbilityFire::ValidateAssociatedAssets()
{

}
#endif

void UEXAbilityFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
}
