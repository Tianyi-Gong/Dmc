#pragma once

#include "CoreMinimal.h"

DECLARE_STATS_GROUP(TEXT("Goap"), STATGROUP_Goap, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("Goap - Agent Tick"), STAT_GoapAgentTick, STATGROUP_Goap);
DECLARE_CYCLE_STAT(TEXT("Goap - Plan Action"), STAT_GoapPlanAction, STATGROUP_Goap);