#include "ZombieExploreState.h"

#include "ZombieExplorationHelper.h"
#include "ZombieMovementHelper.h"

void FZombieExploreState::Execute(AActor* Owner, float& TimeSinceLastExploreMove, float ExploreMoveInterval, float ExploreRadius)
{
	TimeSinceLastExploreMove += Owner ? Owner->GetWorld()->GetDeltaSeconds() : 0.f;

	if (TimeSinceLastExploreMove < ExploreMoveInterval)
	{
		return;
	}

	TimeSinceLastExploreMove = FMath::FRandRange(-1.5f, 0.f);
	const FVector ExploreLocation = FZombieExplorationHelper::GetRandomExploreLocation(Owner, ExploreRadius);
	FZombieMovementHelper::MoveToLocation(Owner, ExploreLocation, 100.f);
}