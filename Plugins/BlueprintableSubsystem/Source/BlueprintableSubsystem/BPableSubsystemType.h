// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

static FPrimaryAssetType const PrimaryAssetType_BPableSubsystem = TEXT("BPableSubsystem");

DECLARE_LOG_CATEGORY_EXTERN(LogBPableSubsystem,Log,All);
DEFINE_LOG_CATEGORY(LogBPableSubsystem);
// #define BPable_Subsystem_PrimaryAssetType FPrimaryAssetType("BPable_Subsystem")
// #define BPable_Subsystem_PrimaryAssetId(InAssetName) FPrimaryAssetId(BPable_Subsystem_PrimaryAssetType,InAssetName)
