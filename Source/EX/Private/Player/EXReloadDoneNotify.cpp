// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXReloadDoneNotify.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXWeapon.h"
#include "Inventory/EXInventoryComponent.h"

FString UEXReloadDoneNotify::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

#if WITH_EDITOR
void UEXReloadDoneNotify::ValidateAssociatedAssets()
{
}
#endif

void UEXReloadDoneNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	/*AEXCharacter* Player = Cast<AEXCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		USkeletalMeshComponent* WeaponMesh = Player->GetWeaponMesh();
		const int32 BoneIdx = WeaponMesh->GetBoneIndex(FName("Clip_Bone"));
		WeaponMesh->UnHideBone(BoneIdx);
	}*/
}
