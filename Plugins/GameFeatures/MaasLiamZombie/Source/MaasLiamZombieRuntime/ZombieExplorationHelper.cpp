#include "ZombieExplorationHelper.h"
#include "StudentPerceptor.h"

FVector FZombieExplorationHelper::GetRandomExploreLocation(AActor* Owner, float ExploreRadius)
{
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	const FVector CurrentLocation = Owner->GetActorLocation();
	const FVector RandomDirection = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal();
	return CurrentLocation + RandomDirection * ExploreRadius;
}

AActor* FZombieExplorationHelper::GetClosestUnsearchedHouse(UStudentPerceptor* Perceptor, AActor* Owner, const TArray<AActor*>& SearchedHouses)
{
	if (!Perceptor || !Owner)
	{
		return nullptr;
	}

	AActor* ClosestHouse = nullptr;
	float ClosestDistance = FLT_MAX;
	const FVector OwnerLocation = Owner->GetActorLocation();

	for (AActor* House : Perceptor->SeenHouses)
	{
		if (!IsValid(House)) continue;
		if (HasHouseBeenSearched(House, SearchedHouses)) continue;

		const float Distance = FVector::Dist(OwnerLocation, House->GetActorLocation());

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestHouse = House;
		}
	}

	return ClosestHouse;
}

bool FZombieExplorationHelper::HasHouseBeenSearched(AActor* House, const TArray<AActor*>& SearchedHouses)
{
	return SearchedHouses.Contains(House);
}