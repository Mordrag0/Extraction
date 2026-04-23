// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/EXHUDWidget.h"
#include "Player/EXCharacter.h"
#include "Online/EXPlayerState.h"
#include "Player/EXPlayerController.h"
#include "Inventory/EXWeapon.h"
#include "Materials/MaterialInstance.h"
#include "HUD/EXExpNotification.h"
#include "HUD/EXKillFeedRow.h"
#include "HUD/EXNotificationList.h"
#include "HUD/EXUIMessage.h"
#include "HUD/EXInputBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Components/Border.h"
#include "Player/EXBaseController.h"
#include "Online/EXObjectiveMessage.h"
#include "Online/EXGameMessage.h"
#include "Online/EXChatMessage.h"
#include "HUD/EXMessageRow.h"
#include "Online/EXChatStatusMessage.h"
#include "HUD/EXGameStatus.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "HUD/EXAssaultCourseWidget.h"
#include "HUD/EXAssaultCourseScores.h"
#include "System/EXGameplayStatics.h"
#include "Online/EXLevelRules.h"
#include "System/EXInteract.h"
#include "HUD/EXPrimaryObjProgress.h"
#include "HUD/EXHUDHealthBar.h"
#include "Inventory/EXInventoryComponent.h"
#include "Player/EXLocalPlayer.h"
#include "Player/EXLocalPlayer.h"
#include "EX.h"
#include "Player/EXPersistentUser.h"
#include "HUD/EXHUDAmmo.h"
#include "HUD/EXHUDAbilities.h"
#include "HUD/EXHUDTimer.h"
#include "HUD/EXHUDProgressbar.h"
#include "HUD/EXHUDElement.h"
#include "HUD/EXDeathScreen.h"
#include "Online/EXVoteMessage.h"
#include "HUD/EXHUDVote.h"

#define LOCTEXT_NAMESPACE "HUD"

void UEXHUDWidget::SetOwner(AEXBaseController* InOwner)
{
	BaseController = InOwner;
	AEXPlayerState* EXPS = InOwner->GetPlayerState<AEXPlayerState>();
	if (!EXPS)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UEXHUDWidget::SetOwner, InOwner));
		return;
	}

	if (GameStatus)
	{
		GameStatus->SetOwner(EXPS);
		bEXInitialized = true;
		UEXGameplayStatics::RefreshTeamColors(this);
	}
}

void UEXHUDWidget::DisplayMessage(FText Message)
{
	if (Announcements) 
	{
		Announcements->SetMessage(Message);
	}
}

void UEXHUDWidget::SetCharacter(AEXCharacter* InCharacter)
{
	EXCharacter = InCharacter;

	if (HUDHealthBar)
	{
		HUDHealthBar->SetCharacter(EXCharacter);
	}

}

void UEXHUDWidget::SetAbilities(UEXInventoryComponent* InventoryComponent)
{
	Abilities->Init(InventoryComponent);
}

void UEXHUDWidget::SetInteractProgressVisibility(bool bVisible)
{
	InteractProgressBar->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UEXHUDWidget::SetInteractProgress(float Ratio)
{
	InteractProgressBar->SetPercent(Ratio); 
}

void UEXHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HitMarkerValue < 3.f)
	{
		HitMarkerValue += InDeltaTime * HitMarkerSpeed;
		MID_HitMarker->SetScalarParameterByIndex(HitMarkerSizeIdx, HitMarkerValue);
	}
}

void UEXHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	DynamicCrosshair = UWidgetBlueprintLibrary::GetDynamicMaterial(CrosshairImage->Brush);

	// Init crosshair from settings
	{
		UEXLocalPlayer* LP = GetWorld()->GetFirstLocalPlayerFromController<UEXLocalPlayer>();
		UEXPersistentUser* PU = LP ? LP->GetPersistentUser() : nullptr;
		if (PU)
		{
			FCrosshair ActiveCrosshair = PU->GetCrosshairs()[PU->GetActiveCrosshairIdx()];
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_GAP_NAME, ActiveCrosshair.CP_Gap);
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_SIZE_NAME, ActiveCrosshair.CP_Size);
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_THICKNESS_NAME, ActiveCrosshair.CP_Thickness);
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_PLUS_DYNAMIC_MULTIPLIER_NAME, 
				(ActiveCrosshair.CP_Size <= 0.f) ? 0.f : ActiveCrosshair.CP_DynamicMultiplier);
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_DOT_SIZE_NAME, ActiveCrosshair.CD_Size);
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_SIZE_NAME, ActiveCrosshair.CC_Size);
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_THICKNESS_NAME, ActiveCrosshair.CC_Thickness);
			DynamicCrosshair->SetScalarParameterValue(CROSSHAIR_PARAM_CIRCLE_DYNAMIC_MULTIPLIER_NAME,
				(ActiveCrosshair.CC_Size <= 0.f) ? 0.f : ActiveCrosshair.CC_DynamicMultiplier);
			DynamicCrosshair->SetVectorParameterValue(CROSSHAIR_PARAM_COLOR_NAME, ActiveCrosshair.CX_Color);
		}
	}

	if (ChatWidget)
	{
		ChatWidget->SetHUD(this);
	}

	MID_HitMarker = UMaterialInstanceDynamic::Create(HitMarkerMaterial, this);
	HitMarker->SetBrushFromMaterial(MID_HitMarker);
	MID_HitMarker->InitializeScalarParameterAndGetIndex(FName("Size"), 100.f, HitMarkerSizeIdx);
}

void UEXHUDWidget::SetDeathScreenVisibility(bool bVisible)
{
	DeathScreen->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UEXHUDWidget::SetOwnerInSpectate(bool bSpec)
{
	DeathScreen->SetOwnerInSpectate(bSpec);
}

void UEXHUDWidget::ChangeMercSelection(int32 MercIdx)
{
	DeathScreen->MercSelectionChanged(MercIdx);
}

void UEXHUDWidget::SetCanRespawn(bool bCanRespawn)
{
	DeathScreen->SetCanRespawn(bCanRespawn);
}

void UEXHUDWidget::SetIcons(const TArray<TSoftClassPtr<AEXCharacter>>& PlayerClasses)
{
	DeathScreen->SetIcons(PlayerClasses);
}

void UEXHUDWidget::ToggleSpeaking(bool bSpeaking)
{
	if (PushToTalkIcon) 
	{
		PushToTalkIcon->SetVisibility(bSpeaking ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UEXHUDWidget::ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, int32 MessageIndex, FText LocalMessageText, UObject* OptionalObject)
{
	// #Messages
	if ((MessageClass == UEXObjectiveMessage::StaticClass())
		|| (MessageClass == UEXGameMessage::StaticClass()))
	{
		if(Announcements)
		{
			Announcements->ReceiveLocalMessage(MessageClass, RelatedPlayerState_1, RelatedPlayerState_2, MessageIndex, LocalMessageText, OptionalObject);
		}
	}
	else if ((MessageClass == UEXChatMessage::StaticClass())
			 || (MessageClass == UEXChatStatusMessage::StaticClass()))
	{
		if(ChatWidget)
		{
			ChatWidget->ReceiveLocalMessage(MessageClass, RelatedPlayerState_1, RelatedPlayerState_2, MessageIndex, LocalMessageText, OptionalObject);
		}
	}
}

void UEXHUDWidget::ShowHitMarker()
{
	HitMarkerValue = 0.f;
}

void UEXHUDWidget::InitAssaultCourse()
{
	if (AssaultCourseWidget)
	{
		AssaultCourseWidget->Init();
	}
}

void UEXHUDWidget::UpdateVote(int32 Yeses, int32 Nos)
{
	if (VoteWidget) 
	{
		VoteWidget->UpdateVotes(Yeses, Nos);
	}
}

void UEXHUDWidget::VoteKick(const FString& PlayerName, const int32 RequiredCount, float VoteDuration)
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("PlayerName"), FText::FromString(PlayerName));
	Args.Add(TEXT("RequiredCount"), FText::FromString(FString::FromInt(RequiredCount)));
	FText VoteText = FText::Format(NSLOCTEXT("HUD", "Vote kick", "Kick {PlayerName}? [{YesVotes}/{RequiredCount}] [{Duration}]"), Args);
	VoteWidget->StartVote(VoteText, VoteDuration);
}

void UEXHUDWidget::VoteShuffle(int32 RequiredCount, float VoteDuration)
{
}

void UEXHUDWidget::Vote(bool bVote)
{
	VoteWidget->Vote(bVote);
}

void UEXHUDWidget::EndVote()
{
	VoteWidget->EndVote();
}

void UEXHUDWidget::OnMatchStateChanged(const FName& MatchState)
{
	MatchStateChanged(MatchState);
}

void UEXHUDWidget::SetGamePaused(bool bPaused)
{
	PauseBorder->SetVisibility(bPaused ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UEXHUDWidget::OnScoreAdded(int32 AddedScore, EScoreType Type)
{
	if (ExpNotifications)
	{
		UEXExpNotification* Row = Cast<UEXExpNotification>(ExpNotifications->AddRow());
		Row->InitRow(AddedScore, Type);
	}
}

void UEXHUDWidget::AddToKillFeed(const FString& Player, UTexture2D* KillIcon, const FString& Killer)
{
	if (KillFeedNotifications) 
	{
		UEXKillFeedRow* Row = Cast<UEXKillFeedRow>(KillFeedNotifications->AddRow());

		AEXPlayerState* EXPS = BaseController->GetPlayerState<AEXPlayerState>();

		Row->InitRow(Player, KillIcon, Killer, EXPS->GetPlayerName());
	}
}

void UEXHUDWidget::SetSpotted(bool bSpotted)
{
	if (SpottedNotification)
	{
		SpottedNotification->SetVisibility(bSpotted ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

void UEXHUDWidget::SetInteractIconVisible(bool bVisible)
{
	InteractIcon->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UEXHUDWidget::SetSpread(float Val)
{
	DynamicCrosshair->SetScalarParameterValue("Spread", Val);
}

void UEXHUDWidget::UpdateAmmo(const UEXWeapon* Weapon)
{
	Ammo->UpdateAmmo(Weapon);
}

void UEXHUDWidget::UpdateInventory(const UEXInventory* Inventory)
{
	Abilities->UpdateInventory(Inventory);
}

void UEXHUDWidget::SetScopeVisibility(bool bVisible)
{
	Scope->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UEXHUDWidget::SetCrosshairVisibility(bool bVisible)
{
	CrosshairImage->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void UEXHUDWidget::OnTeamChanged(AEXTeam* InTeam)
{
	K2_OnTeamChanged(InTeam);
}

void UEXHUDWidget::MatchStateTimeUpdate(int32 InSpawnSeconds, int32 InRoundSeconds)
{
	if (SpawnTimer)
	{
		SpawnTimer->SetText(FText::FromString(UEXGameplayStatics::SecondsToText(InSpawnSeconds)));
	}
	if(RoundTimer)
	{
		RoundTimer->SetText(FText::FromString(UEXGameplayStatics::SecondsToText(InRoundSeconds)));
	}
}

#undef LOCTEXT_NAMESPACE
