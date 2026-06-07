#include "ZombieSearchHouseStateMaasLiam.h"

#include "StudentPerceptorMaasLiam.h"
#include "ZombieExplorationHelperMaasLiam.h"
#include "ZombieMovementHelperMaasLiam.h"
#include "ZombieAgentBrainComponentMaasLiam.h"

void FZombieSearchHouseStateMaasLiam::Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, TArray<AActor*>& SearchedHouses, float HouseSearchAcceptanceRadius)
{
	AActor* ClosestHouse = FZombieExplorationHelperMaasLiam::GetClosestUnsearchedHouse(Perceptor, Owner, SearchedHouses);

	if (!ClosestHouse || !Owner)
	{
		return;
	}

	const float DistanceToHouse = FVector::Dist(Owner->GetActorLocation(), ClosestHouse->GetActorLocation());

	if (DistanceToHouse <= HouseSearchAcceptanceRadius + 100.f)
	{
		if (Perceptor && Perceptor->SeenItems.Num() > 0)
		{
			return;
		}

		if (!SearchedHouses.Contains(ClosestHouse))
		{
			SearchedHouses.Add(ClosestHouse);
		}

		if (UZombieAgentBrainComponentMaasLiam* Brain = Cast<UZombieAgentBrainComponentMaasLiam>(Owner->GetComponentByClass(UZombieAgentBrainComponentMaasLiam::StaticClass())))
		{
			Brain->StartVillageSweep(ClosestHouse->GetActorLocation());
		}

		if (Perceptor)
		{
			Perceptor->SeenHouses.Remove(ClosestHouse);
		}

		return;
	}

	FZombieMovementHelperMaasLiam::MoveToActor(Owner, ClosestHouse, HouseSearchAcceptanceRadius);
}