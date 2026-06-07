#include "ZombieExploreStateMaasLiam.h"

#include "ZombieExplorationHelperMaasLiam.h"
#include "ZombieMovementHelperMaasLiam.h"

void FZombieExploreStateMaasLiam::Execute(AActor* Owner, float& TimeSinceLastExploreMove, float ExploreMoveInterval, float ExploreRadius, 
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

	const FVector ExploreLocation = FZombieExplorationHelperMaasLiam::GetBestExploreLocation(Owner, ExploreRadius, RecentlyExploredLocations, ExploreCandidateCount);

	RecentlyExploredLocations.Add(ExploreLocation);

	while (RecentlyExploredLocations.Num() > MaxRecentExploreLocations)
	{
		RecentlyExploredLocations.RemoveAt(0);
	}

	FZombieMovementHelperMaasLiam::MoveToLocation(Owner, ExploreLocation, 100.f);
}