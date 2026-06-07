#include "ZombieExploreState.h"

#include "ZombieExplorationHelper.h"
#include "ZombieMovementHelper.h"

void FZombieExploreState::Execute(AActor* Owner, float& TimeSinceLastExploreMove, float ExploreMoveInterval, float ExploreRadius, 
	TArray<FVector>& RecentlyExploredLocations, int32 MaxRecentExploreLocations, int32 ExploreCandidateCount)
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

	const FVector ExploreLocation = FZombieExplorationHelper::GetBestExploreLocation(Owner, ExploreRadius, RecentlyExploredLocations, ExploreCandidateCount);

	RecentlyExploredLocations.Add(ExploreLocation);

	while (RecentlyExploredLocations.Num() > MaxRecentExploreLocations)
	{
		RecentlyExploredLocations.RemoveAt(0);
	}

	FZombieMovementHelper::MoveToLocation(Owner, ExploreLocation, 100.f);
}