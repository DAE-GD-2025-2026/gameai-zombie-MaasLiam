#include "ZombieSearchHouseState.h"

#include "StudentPerceptor.h"
#include "ZombieExplorationHelper.h"
#include "ZombieMovementHelper.h"

void FZombieSearchHouseState::Execute(AActor* Owner, UStudentPerceptor* Perceptor, TArray<AActor*>& SearchedHouses, float HouseSearchAcceptanceRadius)
{
	AActor* ClosestHouse = FZombieExplorationHelper::GetClosestUnsearchedHouse(Perceptor, Owner, SearchedHouses);

	if (!ClosestHouse || !Owner)
	{
		return;
	}

	const float DistanceToHouse = FVector::Dist(Owner->GetActorLocation(), ClosestHouse->GetActorLocation());

	if (DistanceToHouse <= HouseSearchAcceptanceRadius + 100.f)
	{
		if (!SearchedHouses.Contains(ClosestHouse))
		{
			SearchedHouses.Add(ClosestHouse);
		}

		if (Perceptor)
		{
			Perceptor->SeenHouses.Remove(ClosestHouse);
		}

		return;
	}

	FZombieMovementHelper::MoveToActor(Owner, ClosestHouse, HouseSearchAcceptanceRadius);
}