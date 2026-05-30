#include "ZombieExploreState.h"

#include "ZombieExplorationHelper.h"
#include "ZombieMovementHelper.h"

void FZombieExploreState::Execute(AActor* Owner, float& TimeSinceLastExploreMove, float ExploreMoveInterval, float ExploreRadius)
{
	if (!Owner)
	{
		return;
	}

	TimeSinceLastExploreMove += Owner->GetWorld()->GetDeltaSeconds();

	if (TimeSinceLastExploreMove < ExploreMoveInterval)
	{
		return;
	}

	TimeSinceLastExploreMove = FMath::FRandRange(-1.5f, 0.f);

	FZombieMovementHelper::MoveToLocation(Owner, FZombieExplorationHelper::GetRandomExploreLocation(Owner, ExploreRadius), 100.f);
}