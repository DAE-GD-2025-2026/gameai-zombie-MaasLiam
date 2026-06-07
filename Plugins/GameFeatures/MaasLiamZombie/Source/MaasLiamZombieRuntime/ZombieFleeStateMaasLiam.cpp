#include "ZombieFleeStateMaasLiam.h"

#include "ZombieThreatHelperMaasLiam.h"
#include "ZombieMovementHelperMaasLiam.h"

void FZombieFleeStateMaasLiam::Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, float FleeDistance)
{
	AActor* ClosestZombie = FZombieThreatHelperMaasLiam::GetClosestZombie(Perceptor, Owner);

	if (!ClosestZombie)
	{
		return;
	}

	const FVector FleeLocation = FZombieThreatHelperMaasLiam::GetFleeLocation(Perceptor, Owner, FleeDistance);

	FZombieMovementHelperMaasLiam::MoveToLocation(Owner, FleeLocation, 100.f);
}