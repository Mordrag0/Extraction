// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EXBaseChatWidget.h"
#include "EXHUDInteractNotify.h"
#include "EXHUDWidget.generated.h"

class AEXPlayerController;
class AEXTeam;
class UTextBlock;
class UEXWeapon;
class UProgressBar;
class UCanvasPanel;
class UImage;
class UEXExpNotification;
class UVerticalBox;
class UTexture;
class UEXKillFeedRow;
class UUniformGridPanel;
class UCanvasPanelSlot;
class UEXNotificationList;
class UBorder;
class UEXGameStatus;
class AEXBaseController;
class UEXBaseChatWidget;
class UEXMessageRow;
class UEXAssaultCourseWidget;
class UEXAssaultCourseScores;
class UEXPrimaryObjProgress;
class AEXLevelRules;
class AEXCharacter;
class UEXHUDHealthBar;
class UEXAbilityIcon;
class UEXInventoryComponent;
class UEXHUDAmmo;
class UEXHUDAbilities;
class UEXHUDTimer;
class UEXHUDProgressbar;
class UEXHUDElement;
class UEXDeathScreen;
class UEXHUDVote;
class UEXTextSelectPanel;

enum class EScoreType : uint8;

/**
 * 
 */
UCLASS()
class EX_API UEXHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetOwner(AEXBaseController* InOwner);

	AEXBaseController* GetController() const { return BaseController; }

	void DisplayMessage(FText Message);

	void SetCharacter(AEXCharacter* InCharacter);

	void SetAbilities(UEXInventoryComponent* InventoryComponent);

	UFUNCTION()
	void SetInteractProgressVisibility(bool bVisible);

	UFUNCTION()
	void SetInteractProgress(float Ratio);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnTeamChanged(AEXTeam* InTeam);

	UFUNCTION()
	void MatchStateTimeUpdate(int32 InSpawnSeconds, int32 InRoundSeconds);

	UFUNCTION()
	void UpdateAmmo(const UEXWeapon* Weapon);

	UFUNCTION()
	void UpdateInventory(const UEXInventory* Inventory);

	void SetScopeVisibility(bool bVisible);
	void SetCrosshairVisibility(bool bVisible);

	void SetSpread(float Val);

	void SetInteractIconVisible(bool bVisible);

	void OnScoreAdded(int32 AddedScore, EScoreType Type);

	void AddToKillFeed(const FString& Player, UTexture2D* KillIcon, const FString& Killer);

	void SetSpotted(bool bSpotted);

	void OnMatchStateChanged(const FName& MatchState);

	void SetGamePaused(bool bPaused);

	void ToggleSpeaking(bool bSpeaking);

	UEXTextSelectPanel* GetQuickChatWindow() const { return QuickChatWindow; }

	UEXTextSelectPanel* GetAdminPanel() const { return AdminPanel; }

	UEXGameStatus* GetGameStatus() const { return GameStatus; }

	void ReceiveLocalMessage(TSubclassOf<class UEXLocalMessage> MessageClass, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, int32 MessageIndex, FText LocalMessageText, UObject* OptionalObject);

	UEXBaseChatWidget* GetChatWidget() const { return ChatWidget; }

	UEXHUDHealthBar* GetHUDHealthBar() const { return HUDHealthBar; }

	void ShowHitMarker();

	void InitAssaultCourse();

	UEXPrimaryObjProgress* GetPrimaryObjProgressWidget() const { return PrimaryObjProgress; }

	UEXAssaultCourseScores* GetAssaultCourseScores() const { return AssaultCourseScores; }

	bool IsInitialized() const { return bEXInitialized; }

	void UpdateVote(int32 Yeses, int32 Nos);
	void VoteKick(const FString& PlayerName, const int32 Count, float VoteDuration);
	void VoteShuffle(int32 Count, float VoteDuration);
	void Vote(bool bVote);
	void EndVote();
protected:

	FTimerHandle TimerHandle_Message;

	UPROPERTY()
	AEXBaseController* BaseController = nullptr;

	UPROPERTY()
	AEXCharacter* EXCharacter = nullptr;

	bool bEXInitialized = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD", Meta = (DisplayName = "On Init", ScriptName = "OnInit"))
	void K2_OnInit(AEXPlayerState* PS);
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD", Meta = (DisplayName = "On Team Changed", ScriptName = "K2_OnTeamChanged"))
	void K2_OnTeamChanged(AEXTeam* InTeam);

	UFUNCTION(BlueprintImplementableEvent, Category = "Match")
	void MatchStateChanged(const FName& MatchState);


	UPROPERTY(BlueprintReadOnly, Transient)
	bool bShowInteractProgress;

	UPROPERTY(BlueprintReadOnly, Transient)
	float InteractProgress;

	virtual void NativeConstruct() override;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicCrosshair = nullptr;

	/// Hit Marker

	UPROPERTY(Meta = (BindWidget))
	UImage* HitMarker = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	UMaterialInstance* HitMarkerMaterial = nullptr;
	UPROPERTY()
	UMaterialInstanceDynamic* MID_HitMarker = nullptr;

	float HitMarkerValue = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	float HitMarkerSpeed = .05f;
	int32 HitMarkerSizeIdx = -1;
	/// Hit Marker

	/// Death Screen
public:
	void SetDeathScreenVisibility(bool bVisible);
	void SetOwnerInSpectate(bool bSpec);
	void ChangeMercSelection(int32 MercIdx);
	void SetCanRespawn(bool bRespawn);
	void SetIcons(const TArray<TSoftClassPtr<AEXCharacter>>& ClassesToLoad);
protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXDeathScreen* DeathScreen = nullptr;

	/// Death Screen

protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UCanvasPanel* Scope = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UImage* CrosshairImage = nullptr;

	UPROPERTY(Meta = (BindWidgetOptional))
	UEXAssaultCourseWidget* AssaultCourseWidget = nullptr;
	UPROPERTY(Meta = (BindWidgetOptional))
	UEXAssaultCourseScores* AssaultCourseScores = nullptr;


	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UBorder* PauseBorder = nullptr;

public:
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXHUDTimer* SpawnTimer = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXHUDTimer* RoundTimer = nullptr;


	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXHUDAmmo* Ammo = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXHUDAbilities* Abilities = nullptr;


	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXHUDInteractNotify* InteractIcon = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXHUDProgressbar* InteractProgressBar = nullptr;

	// Exp
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXNotificationList* ExpNotifications = nullptr;

	// Killfeed
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXNotificationList* KillFeedNotifications = nullptr;

	// PushToTalkIcon
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXHUDElement* PushToTalkIcon = nullptr;

	// QuickChatWindow
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXTextSelectPanel* QuickChatWindow = nullptr;

	// Game status icons
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXGameStatus* GameStatus = nullptr;

	// Chat Widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXBaseChatWidget* ChatWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXMessageRow* Announcements = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXPrimaryObjProgress* PrimaryObjProgress = nullptr;

	//HealthBar widget
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXHUDHealthBar* HUDHealthBar = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXHUDElement* SpottedNotification = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidgetOptional))
	UEXHUDVote* VoteWidget = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD", Meta = (BindWidget))
	UEXTextSelectPanel* AdminPanel = nullptr;

};
