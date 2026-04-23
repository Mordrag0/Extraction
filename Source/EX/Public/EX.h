// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Public/CollisionQueryParams.h"

#define CLIENT_VERSION "0.0.1"

#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_WeaponTrace ECollisionChannel::ECC_GameTraceChannel2
#define ECC_WorldOnly ECollisionChannel::ECC_GameTraceChannel3
#define ECC_Visibility_Simple ECollisionChannel::ECC_GameTraceChannel5
#define ECC_Melee ECollisionChannel::ECC_GameTraceChannel6

#define ECC_Ability ECollisionChannel::ECC_GameTraceChannel4
#define ECC_Objective ECollisionChannel::ECC_GameTraceChannel7
#define ECC_Interact ECollisionChannel::ECC_GameTraceChannel8

// #LOGS
DECLARE_LOG_CATEGORY_EXTERN(LogEXGameMode, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXGameState, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXChar, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXLevel, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXWeapon, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXAbility, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXInventory, Warning, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXController, Error, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXObjective, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXAnim, Error, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXTeam, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXAssaultCourse, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXUI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXAdmin, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXBeacon, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXOnline, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXUIDebug, Fatal, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXAssets, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEXPlayer, Log, All);

const int32 MAX_PLAYER_NAME_LENGTH = 16;
const int32 MAX_CHAT_TEXT_SIZE = 384;


//For UE4 Profiler ~ Stat Group
DECLARE_STATS_GROUP(TEXT("EX"), STATGROUP_EX, STATCAT_Advanced);

// #DESYNC
#define MAX_VARIANCE_ALLOWED .05f

// #INPUT
#define SCOREBOARD_KEY "Tab"
#define ESCAPE_KEY "Escape"

// #MATERIAL INSTANCE PARAM NAMES

#define CROSSHAIR_PARAM_PLUS_GAP_NAME "PlusGap"
#define CROSSHAIR_PARAM_PLUS_SIZE_NAME "PlusSize"
#define CROSSHAIR_PARAM_PLUS_THICKNESS_NAME "PlusThickness"
#define CROSSHAIR_PARAM_PLUS_DYNAMIC_MULTIPLIER_NAME "PlusSpreadMultiplier"

#define CROSSHAIR_PARAM_DOT_SIZE_NAME "DotSize"
#define CROSSHAIR_PARAM_CIRCLE_SIZE_NAME "CircleSize"
#define CROSSHAIR_PARAM_CIRCLE_THICKNESS_NAME "CircleThickness"
#define CROSSHAIR_PARAM_CIRCLE_DYNAMIC_MULTIPLIER_NAME "CircleSpreadMultiplier"

#define CROSSHAIR_PARAM_COLOR_NAME "Color"

#define CROSSHAIR_PARAM_EDITOR_WIDTH "Width"
#define CROSSHAIR_PARAM_EDITOR_HEIGHT "Heigth"

// #MAPS
const TArray<FName> ValidMapNames = { FName("Map3"), FName("DM_EX"), FName("ObjTest"), FName("Map10"), FName("Map11"), }; // #EXTODO2 remove, get from master server / config

// #UI
const int32 TRAVEL_WIDGET_Z_ORDER = 300;
const int32 LOADING_MAP_WIDGET_Z_ORDER = 301;

// #EDITORDEFAULTS
#define USE_MASTER_SERVER_WITH_EDITOR false
