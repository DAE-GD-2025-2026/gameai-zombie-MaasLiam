#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

class FZombieExplorationHelper
{
public:
	static FVector GetRandomExploreLocation(AActor* Owner, float ExploreRadius);
	static AActor* GetClosestUnsearchedHouse(UStudentPerceptor* Perceptor, AActor* Owner, const TArray<AActor*>& SearchedHouses);
	static bool HasHouseBeenSearched(AActor* House, const TArray<AActor*>& SearchedHouses);
};