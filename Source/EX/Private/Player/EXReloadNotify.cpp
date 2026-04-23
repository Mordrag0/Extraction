// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EXReloadNotify.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXWeapon.h"
#include "Inventory/EXInventoryComponent.h"

FString UEXReloadNotify::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

#if WITH_EDITOR
void UEXReloadNotify::ValidateAssociatedAssets()
{
}
#endif

void UEXReloadNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	/*AEXCharacter* Player = Cast<AEXCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		USkeletalMeshComponent* WeaponMesh = Player->GetWeaponMesh();
		const int32 BoneIdx = WeaponMesh->GetBoneIndex(FName("Clip_Bone"));
		WeaponMesh->HideBone(BoneIdx, EPhysBodyOp::PBO_None);
	}*/
}
