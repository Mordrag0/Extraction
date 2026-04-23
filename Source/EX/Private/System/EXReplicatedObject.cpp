// Fill out your copyright notice in the Description page of Project Settings.


#include "System/EXReplicatedObject.h"
#include "Player/EXCharacter.h"

UWorld* UEXReplicatedObject::GetWorld() const
{
	if (const UObject* MyOuter = GetOuter())
	{
		return MyOuter->GetWorld();
	}
	return nullptr;
}

int32 UEXReplicatedObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	check(GetOuter());
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool UEXReplicatedObject::CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));

	AActor* Owner = GetOwningActor();
	if (UNetDriver* NetDriver = Owner->GetNetDriver())
	{
		NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}

void UEXReplicatedObject::Destroy()
{
	if (IsValid(this))
	{
		checkf(GetOwningActor()->HasAuthority() == true, TEXT("Destroy:: Object does not have authority to destroy itself!"));

		OnDestroyed();
		MarkAsGarbage();
	}
}

bool UEXReplicatedObject::IsLocallyOwned() const
{
	const AEXCharacter* Owner = Cast<AEXCharacter>(GetOwningActor());
	return (IsOwnedByLocalRemote() || (Owner && Owner->IsLocallyControlled()));
}

bool UEXReplicatedObject::IsNonOwningAuthority() const
{
	return (IsAuthority() && !IsLocallyOwned());
}
