#include "ZombieExplorationHelperMaasLiam.h"
#include "StudentPerceptorMaasLiam.h"

FVector FZombieExplorationHelperMaasLiam::GetRandomExploreLocation(AActor* Owner, float ExploreRadius)
{
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	const FVector CurrentLocation = Owner->GetActorLocation();
	const FVector RandomDirection = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal();
	return CurrentLocation + RandomDirection * ExploreRadius;
}

FVector FZombieExplorationHelperMaasLiam::GetBestExploreLocation(AActor* Owner, float ExploreRadius, const TArray<FVector>& RecentlyExploredLocations, int32 CandidateCount)
{
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	FVector BestLocation = Owner->GetActorLocation();
	float BestScore = -FLT_MAX;

	for (int32 CandidateIndex = 0; CandidateIndex < CandidateCount; ++CandidateIndex)
	{
		const FVector CandidateLocation = GetRandomExploreLocation(Owner, ExploreRadius);

		const float CandidateScore = ScoreExploreLocation(CandidateLocation, RecentlyExploredLocations);

		if (CandidateScore > BestScore)
		{
			BestScore = CandidateScore;
			BestLocation = CandidateLocation;
		}
	}

	return BestLocation;
}

float FZombieExplorationHelperMaasLiam::ScoreExploreLocation(const FVector& CandidateLocation, const TArray<FVector>& RecentlyExploredLocations)
{
	float Score = 0.f;

	if (RecentlyExploredLocations.Num() == 0)
	{
		return FMath::FRandRange(0.f, 100.f);
	}

	for (const FVector& RecentLocation : RecentlyExploredLocations)
	{
		const float DistanceToRecentLocation = FVector::Dist(CandidateLocation, RecentLocation);
		Score += DistanceToRecentLocation;
	}

	Score += FMath::FRandRange(0.f, 250.f);

	return Score;
}

AActor* FZombieExplorationHelperMaasLiam::GetClosestUnsearchedHouse(UStudentPerceptorMaasLiam* Perceptor, AActor* Owner, const TArray<AActor*>& SearchedHouses)
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

bool FZombieExplorationHelperMaasLiam::HasHouseBeenSearched(AActor* House, const TArray<AActor*>& SearchedHouses)
{
	return SearchedHouses.Contains(House);
}