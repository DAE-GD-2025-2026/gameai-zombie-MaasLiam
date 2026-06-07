#pragma once

#include "CoreMinimal.h"

class UStudentPerceptorMaasLiam;

class FZombieExplorationHelperMaasLiam
{
public:
	static FVector GetRandomExploreLocation(AActor* Owner, float ExploreRadius);
	static AActor* GetClosestUnsearchedHouse(UStudentPerceptorMaasLiam* Perceptor, AActor* Owner, const TArray<AActor*>& SearchedHouses);
	static bool HasHouseBeenSearched(AActor* House, const TArray<AActor*>& SearchedHouses);
	static FVector GetBestExploreLocation(AActor* Owner, float ExploreRadius, const TArray<FVector>& RecentlyExploredLocations, int32 CandidateCount);
	static float ScoreExploreLocation(const FVector& CandidateLocation, const TArray<FVector>& RecentlyExploredLocations);
};