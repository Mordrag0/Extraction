// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/EXPlacedAbility.h"
#include "Kismet/GameplayStatics.h"
#include "Online/EXPlayerState.h"
#include "Online/EXTeam.h"
#include "Player/EXCharacter.h"
#include "Online/EXGameModeBase.h"

AEXPlacedAbility::AEXPlacedAbility()
{
	bReplicates = true;
}

void AEXPlacedAbility::BeginPlay()
{
	Super::BeginPlay();
	SetPlayer(Cast<AEXCharacter>(GetInstigator()));

	Health = MaxHealth;
}

void AEXPlacedAbility::PickUp(AEXCharacter* Player)
{
	if (HasAuthority())
	{
		Inventory->RecoverAbility(Health / MaxHealth);
		Destroy();
	}
}

void AEXPlacedAbility::Remove()
{
	if (HasAuthority())
	{
		Destroy();
	}
}


void AEXPlacedAbility::Die()
{
	if (HasAuthority())
	{
		NetDestroy();
	}
}

float AEXPlacedAbility::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		AEXGameModeBase* GM = GetWorld()->GetAuthGameMode<AEXGameModeBase>();
		if (GM)
		{
			ActualDamage = GM->ModifyDamage(ActualDamage, this, DamageEvent, EventInstigator, DamageCauser);
		}

		Health -= ActualDamage;
		if (Health <= 0.f)
		{
			Die();
		}
	}
	return ActualDamage;
}

void AEXPlacedAbility::SetPlayer(AEXCharacter* Player)
{
	PlayerOwner = Player;
	if (!IsNetMode(NM_Standalone))
	{
		Team = Player->GetPlayerState<AEXPlayerState>()->GetTeam();
	}
}


