// Fill out your copyright notice in the Description page of Project Settings.

#include "System/EXGameplayStatics.h"
#include "EX.h"
#include "Misc/EXExplosionEffect.h"
#include "Player/EXLocalPlayer.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/EXWeapon.h"
#include "UObject/UObjectIterator.h"
#include "Player/EXCharacter.h"
#include "Inventory/EXInventory.h"
#include "Online/EXTeam.h"
#include "Online/EXPlayerState.h"
#include "Online/EXLevelRules.h"
#include "steam/steam_api.h"
#include "Player/EXBaseController.h"
#include "Player/EXPlayerController.h"
#include "HUD/EXHUDWidget.h"
#include "Online/EXGameStateSW.h"
#include "Online/EXGameStateDM.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/EXMusicManager.h"
#include "Misc/EXInputInfo.h"
#include "CollisionQueryParams.h"
#include "WorldCollision.h"

#if !WITH_EDITOR
AEXLevelRules* UEXGameplayStatics::LevelRules = nullptr;
#endif
float UEXGameplayStatics::MasterVolume = 1.f;
float UEXGameplayStatics::MusicVolume = 1.f;


bool UEXGameplayStatics::SameTeam(AEXCharacter* EXCharacter1, AEXCharacter* EXCharacter2)
{
	if (!ensure(EXCharacter1 && EXCharacter2))
	{
		return false;
	}
	return EXCharacter1->GetPlayerState<AEXPlayerState>()->OnSameTeam(EXCharacter2->GetPlayerState<AEXPlayerState>());
}

TArray<AEXCharacter*> UEXGameplayStatics::GetTeammates(AEXCharacter* Character)
{
	TArray<AEXCharacter*> Teammates;
	AEXPlayerState* EXPS = Character->GetEXPlayerState();
	AEXTeam* Team = EXPS ? EXPS->GetTeam() : nullptr;
	if (!Team)
	{
		return Teammates;
	}

	TArray<AEXPlayerState*> TeamPlayerStates = Team->GetMembers();
	for (AEXPlayerState* PS : TeamPlayerStates)
	{
		if (PS == EXPS)
		{
			continue;
		}
		AEXCharacter* Pawn = PS->GetPawn<AEXCharacter>();
		if (Pawn)
		{
			Teammates.Add(Pawn);
		}
	}

	return Teammates;
}

void UEXGameplayStatics::RefreshTeamColors(UObject* WCO, bool bTest)
{
	if (bTest)
	{
		UE_LOG(LogTemp, Warning, TEXT("refresh1"));
	}
	if (!IsValid(WCO))
	{
		return;
	}
	UWorld* World = WCO->GetWorld();
	if (World->GetGameState<AEXGameStateDM>())
	{
		// No team colors in DM
		return;
	}
	if (bTest)
	{
		UE_LOG(LogTemp, Warning, TEXT("refresh2"));
	}

	AEXPlayerController* LocalPC = World->GetFirstPlayerController<AEXPlayerController>();
	if (!LocalPC
		|| !IsValid(LocalPC)
		|| !LocalPC->HasActorBegunPlay()
		|| !LocalPC->GetHUDWidget()
		|| !LocalPC->GetHUDWidget()->IsInitialized())
	{
		return;
	}
	if (bTest)
	{
		UE_LOG(LogTemp, Warning, TEXT("refresh3"));
	}
	AEXPlayerState* LocalPS = LocalPC->GetPlayerState<AEXPlayerState>();
	if (!LocalPS || !LocalPS->GetTeam())
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateStatic(&UEXGameplayStatics::RefreshTeamColors, WCO, bTest));
		return;
	}
	if (bTest)
	{
		UE_LOG(LogTemp, Warning, TEXT("refresh4"));
	}

	const AEXGameStateSW* GS = World->GetGameState<AEXGameStateSW>();
	if (!GS)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateStatic(&UEXGameplayStatics::RefreshTeamColors, WCO, bTest));
		return;
	}
	if (bTest)
	{
		UE_LOG(LogTemp, Warning, TEXT("refresh5"));
	}

	AEXTeam* Attackers = GS->GetAttackers();
	AEXTeam* Defenders = GS->GetDefenders();
	AEXTeam* Spectators = GS->GetSpectators();
	if (!Attackers || Attackers->GetMembers().Contains(nullptr)
		|| !Defenders || Defenders->GetMembers().Contains(nullptr)
		|| !Spectators || Spectators->GetMembers().Contains(nullptr))
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateStatic(&UEXGameplayStatics::RefreshTeamColors, WCO, bTest));
		return;
	}
	if (bTest)
	{
		UE_LOG(LogTemp, Warning, TEXT("refresh6"));
		UE_LOG(LogTemp, Warning, TEXT("%d"), Attackers->NumMembers());
		for (AEXPlayerState* EXPS : Attackers->GetMembers())
		{
			AEXCharacter* Character = EXPS->GetPawn<AEXCharacter>();
			if (Character)
			{
				UE_LOG(LogEXUI, Warning, TEXT("-> %s"), *UEnum::GetValueAsString(Character->GetStatus()));
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("%d"), Defenders->NumMembers());
		for (AEXPlayerState* EXPS : Defenders->GetMembers())
		{
			AEXCharacter* Character = EXPS->GetPawn<AEXCharacter>();
			if (Character)
			{
				UE_LOG(LogEXUI, Warning, TEXT("-> %s"), *UEnum::GetValueAsString(Character->GetStatus()));
			}
		}
	}
	UEXGameplayStatics::RefreshCharacterColors(Attackers, Defenders);
	LocalPC->RefreshGameStatusWidget(Attackers, Defenders);
}

void UEXGameplayStatics::RefreshCharacterColors(AEXTeam* Attackers, AEXTeam* Defenders)
{
	RefreshCharacterColors(Attackers);
	RefreshCharacterColors(Defenders);
}
void UEXGameplayStatics::RefreshCharacterColors(AEXTeam* Team)
{
	for (AEXPlayerState* PS : Team->GetMembers())
	{
		AEXCharacter* Player = PS ? PS->GetPawn<AEXCharacter>() : nullptr;
		if (Player)
		{
			Player->SetMaterial();
		}
	}
}

FText UEXGameplayStatics::FloatToText(float Value, int Precision)
{
	int32 Whole = FMath::FloorToInt(Value);
	int32 Decimal = FMath::FloorToInt((Value - Whole) * (FMath::Pow(10.f, Precision)));

	FFormatNamedArguments Args;
	Args.Add(FString("Whole"), Whole);
	Args.Add(FString("Decimal"), Decimal);
	return FText::Format(FTextFormat(FText::FromString("{Whole}.{Decimal}")), Args);
}

TArray<TSubclassOf<AEXCharacter>> UEXGameplayStatics::GetSquad(const TArray<EMerc>& Squad)
{
	TArray<TSubclassOf<AEXCharacter>> CharacterSquad;
	for (EMerc Merc : Squad)
	{
		CharacterSquad.Add(TSubclassOf<AEXCharacter>(GetMercCDO(Merc)->StaticClass()));
	}
	return CharacterSquad;
}

AEXLevelRules* UEXGameplayStatics::GetLevelRules(UObject* WCO)
{
#if WITH_EDITOR
	return Cast<AEXLevelRules>(UGameplayStatics::GetActorOfClass(WCO, AEXLevelRules::StaticClass()));
#else
	if (UNLIKELY(!LevelRules)) // Will be null before LevelRules::BeginPlay and after LevelRules::EndPlay are called
	{
		return Cast<AEXLevelRules>(UGameplayStatics::GetActorOfClass(WCO, AEXLevelRules::StaticClass()));
	}
	return LevelRules;
#endif
}

void UEXGameplayStatics::SetLevelRules(AEXLevelRules* InLevelRules)
{
#if !WITH_EDITOR
	LevelRules = InLevelRules;
#endif
}

void UEXGameplayStatics::SetMasterVolume(float InMasterVolume)
{
	MasterVolume = InMasterVolume;
}

void UEXGameplayStatics::SetMusicVolume(float InMusicVolume)
{
	MusicVolume = InMusicVolume;
}

bool UEXGameplayStatics::GetClosestPointOnCollision(const AActor* Actor, const FVector& Point, const ECollisionChannel& Channel, FVector& ClosestPoint)
{
	if (!Actor)
	{
		return false;
	}

	TArray<UPrimitiveComponent*> ComponentList;
	Actor->GetComponents<UPrimitiveComponent>(ComponentList, false);

	float ClosestDistance = -1.f;

	for (UPrimitiveComponent*& Component : ComponentList)
	{
		if (!Component || !Component->IsCollisionEnabled())
		{
			continue;
		}

		if (Component->GetCollisionResponseToChannel(Channel) != ECollisionResponse::ECR_Block)
		{
			continue;
		}

		FVector ComponentPoint;
		const float Distance = Component->GetClosestPointOnCollision(Point, ComponentPoint);

		if ((Distance < 0.f) || ((ClosestDistance >= 0.f) && (Distance > ClosestDistance)))
		{
			continue;
		}

		ClosestPoint = ComponentPoint;
		ClosestDistance = Distance;
	}

	return ClosestDistance >= 0.f;
}

AActor* UEXGameplayStatics::GetActorLookingAt(const AEXCharacter* Player, float MaxTraceRange)
{
	FHitResult HitResult;

	FVector StartLocation;
	FRotator TraceDirection;
	Player->GetActorEyesViewPoint(StartLocation, TraceDirection);

	FVector EndLocation = StartLocation + (TraceDirection.Vector() * MaxTraceRange);

	FCollisionQueryParams CQP = FCollisionQueryParams::DefaultQueryParam;
	CQP.AddIgnoredActor(Player);
	if (!Player->GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Interact, CQP)) 
	{
		return nullptr;
	}

	return HitResult.GetActor();
}

class AEXMusicManager* UEXGameplayStatics::GetMusicManager(UObject* WCO)
{
	return Cast<AEXMusicManager>(UGameplayStatics::GetActorOfClass(WCO, AEXMusicManager::StaticClass()));
}

void UEXGameplayStatics::SpawnExplosion(UWorld* World, TSubclassOf<class AEXExplosionEffect> ExplosionClass, const FTransform Transform, bool bComponentHit, const FHitResult& Impact)
{
	AEXExplosionEffect* const EffectActor = World->SpawnActorDeferred<AEXExplosionEffect>(ExplosionClass, Transform);

	if (EffectActor)
	{
		EffectActor->bComponentHit = bComponentHit;
		EffectActor->SurfaceHit = Impact;
		EffectActor->VolumeMultiplier = UEXGameplayStatics::GetMasterVolume();
		UGameplayStatics::FinishSpawningActor(EffectActor, Transform);
	}
}

UEXInputInfo* UEXGameplayStatics::GetInputInfoCDO()
{
	UEXInputInfo* InputInfo = Cast<UEXInputInfo>(UEXInputInfo::StaticClass()->GetDefaultObject(true));
	return InputInfo;
}

FString UEXGameplayStatics::SecondsToText(int32 Seconds)
{
	const int32 TimerMinutes = Seconds / 60;
	const int32 TimerSeconds = Seconds % 60;
	TArray<FStringFormatArg> StringArgs;
	StringArgs.Add(FStringFormatArg(LeadingZeroes(TimerMinutes)));
	StringArgs.Add(FStringFormatArg(LeadingZeroes(TimerSeconds)));
	FString ResolutionText = FString::Format(TEXT("{0}:{1}"), StringArgs);
	return ResolutionText;
}

FString UEXGameplayStatics::LeadingZeroes(int32 Val)
{
	char Buff[4];
	snprintf(Buff, sizeof(Buff), "%02d", Val);
	return Buff;
}

uint64 UEXGameplayStatics::GetLocalSteamID(UObject* WCO)
{
	uint64 SteamId = 0;

#if WITH_EDITOR
	AEXBaseController* PC = WCO->GetWorld()->GetFirstPlayerController<AEXBaseController>();
	SteamId = PC->GetPlayerState<AEXPlayerState>()->GetSteamID();
#else

	FBPUniqueNetId SteamNetId = UAdvancedSteamFriendsLibrary::GetLocalSteamIDFromSteam();
	const FUniqueNetId* UniqueNetID = SteamNetId.GetUniqueNetId();
	if (UniqueNetID)
	{
		check(UniqueNetID->GetSize() == 8);
		const uint8* IdData = UniqueNetID->GetBytes();
		SteamId = *((uint64*)IdData);
}
#endif

	return SteamId;
}

FString UEXGameplayStatics::GetSteamName(UObject* WCO /*= nullptr*/)
{
	FString SteamName;

#if WITH_EDITOR

	TArray<FStringFormatArg> StringArgs;
	StringArgs.Add(FStringFormatArg(FString::FromInt(GetLocalSteamID(WCO))));
	SteamName = FString::Format(TEXT("EditorPlayer_{0}"), StringArgs);
#else
	FBPUniqueNetId SteamNetId = UAdvancedSteamFriendsLibrary::GetLocalSteamIDFromSteam();
	SteamName = UAdvancedSteamFriendsLibrary::GetSteamPersonaName(SteamNetId);
#endif

	return SteamName;
}

FBPUniqueNetId UEXGameplayStatics::CreateSteamId(uint64 Id)
{
	FBPUniqueNetId netId;

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (SteamAPI_Init())
	{
		TSharedPtr<const FUniqueNetId> SteamID(new const FUniqueNetIdSteam2(Id));
		netId.SetUniqueNetId(SteamID);
	}
#endif

	return netId;
}

bool UEXGameplayStatics::SteamOnline()
{
#if WITH_EDITOR
	return true;
#endif
	return SteamAPI_Init();
}

UEXWeapon* UEXGameplayStatics::GetWeaponCDO(EWeapon WeaponType)
{
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;

		// Only interested in native C++ classes
		if (Class->IsNative())
		{
			continue;
		}

		// Ignore deprecated
		if (Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
		{
			continue;
		}

		//#if WITH_EDITOR
		//		// Ignore skeleton classes (semi-compiled versions that only exist in-editor)
		//		if (FKismetEditorUtilities::IsClassABlueprintSkeleton(Class))
		//		{
		//			continue;
		//		}
		//#endif

				// Check this class is a subclass of Base
				/*if (!Class->IsChildOf(Base))
				{
					continue;
				}*/

		if (!Class->IsChildOf(UEXWeapon::StaticClass()))
		{
			continue;
		}

		UEXWeapon* Weapon = Class->GetDefaultObject<UEXWeapon>();
		if (Weapon->GetWeaponType() == WeaponType)
		{
			return Weapon;
		}
	}
	return nullptr;
}

UEXInventory* UEXGameplayStatics::GetAbilityCDO(EAbility AbilityType)
{
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;

		// Only interested in native C++ classes
		if (Class->IsNative())
		{
			continue;
		}
		// Ignore deprecated
		if (Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
		{
			continue;
		}

		if (!Class->IsChildOf(UEXInventory::StaticClass()))
		{
			continue;
		}


		UEXInventory* Inventory = Class->GetDefaultObject<UEXInventory>();
		if (Inventory->GetAbilityType() == AbilityType)
		{
			return Inventory;
		}
	}
	return nullptr;
}

AEXCharacter* UEXGameplayStatics::GetMercCDO(EMerc MercType)
{
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;

		// Only interested in native C++ classes
		if (Class->IsNative())
		{
			continue;
		}
		// Ignore deprecated
		if (Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
		{
			continue;
		}

		if (!Class->IsChildOf(AEXCharacter::StaticClass()))
		{
			continue;
		}

		AEXCharacter* Merc = Class->GetDefaultObject<AEXCharacter>();
		if (Merc->IsMerc(MercType))
		{
			return Merc;
		}
	}
	return nullptr; // AEXMenuController::MercClassesToLoad
}

void UEXGameplayStatics::ApplyRadialDamage(const UObject* WCO, float BaseDamage, const FVector& Origin, float DamageRadius, TSubclassOf<class UDamageType> DamageTypeClass, const TArray<AActor*>& IgnoreActors, AActor* DamageCauser /*= nullptr*/, AController* InstigatedByController /*= nullptr*/, bool bDoFullDamage /*= false*/, ECollisionChannel DamagePreventionChannel /*= ECC_Visibility_Simple*/)
{
	//FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(EXApplyRadialDamage), false, DamageCauser);
	//SphereParams.AddIgnoredActors(IgnoreActors);
	//UWorld* World = WCO->GetWorld();
	//TArray<FOverlapResult> Overlaps;
	//World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(DamageRadius), SphereParams);

	//// collate into per-actor list of hit components
	//TMap<AActor*, TArray<FHitResult> > OverlapComponentMap;
	//for (int32 Idx = 0; Idx < Overlaps.Num(); ++Idx)
	//{
	//	FOverlapResult const& Overlap = Overlaps[Idx];
	//	AActor* const OverlapActor = Overlap.GetActor();

	//	if (OverlapActor &&
	//		OverlapActor->CanBeDamaged() &&
	//		OverlapActor != DamageCauser &&
	//		Overlap.Component.IsValid())
	//	{
	//		FHitResult Hit;
	//		if (ComponentIsDamageableFrom(Overlap.Component.Get(), Origin, DamageCauser, IgnoreActors, DamagePreventionChannel, Hit))
	//		{
	//			TArray<FHitResult>& HitList = OverlapComponentMap.FindOrAdd(OverlapActor);
	//			HitList.Add(Hit);
	//		}
	//	}
	//}

	//bool bAppliedDamage = false;

	//if (OverlapComponentMap.Num() > 0)
	//{
	//	// make sure we have a good damage type
	//	TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());

	//	FRadialDamageEvent DmgEvent;
	//	DmgEvent.DamageTypeClass = ValidDamageTypeClass;
	//	DmgEvent.Origin = Origin;
	//	DmgEvent.Params = FRadialDamageParams(BaseDamage, MinimumDamage, DamageInnerRadius, DamageRadius, DamageFalloff);

	//	// call damage function on each affected actors
	//	for (TMap<AActor*, TArray<FHitResult> >::TIterator It(OverlapComponentMap); It; ++It)
	//	{
	//		AActor* const Victim = It.Key();
	//		TArray<FHitResult> const& ComponentHits = It.Value();
	//		DmgEvent.ComponentHits = ComponentHits;

	//		Victim->TakeDamage(BaseDamage, DmgEvent, InstigatedByController, DamageCauser);

	//		bAppliedDamage = true;
	//	}
	//}
}
