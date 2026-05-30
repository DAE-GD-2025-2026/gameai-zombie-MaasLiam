#include "ZombieFleeState.h"

#include "ZombieThreatHelper.h"
#include "ZombieMovementHelper.h"

void FZombieFleeState::Execute(AActor* Owner, UStudentPerceptor* Perceptor, float FleeDistance)
{
	AActor* ClosestZombie = FZombieThreatHelper::GetClosestZombie(Perceptor, Owner);

	if (!ClosestZombie)
	{
		return;
	}

	const FVector FleeLocation = FZombieThreatHelper::GetFleeLocation(Perceptor, Owner, FleeDistance);

	FZombieMovementHelper::MoveToLocation(Owner, FleeLocation, 100.f);
}