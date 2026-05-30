#pragma once

#include "CoreMinimal.h"

class FZombieExploreState
{
public:
	static void Execute(AActor* Owner, float& TimeSinceLastExploreMove, float ExploreMoveInterval, float ExploreRadius);
};