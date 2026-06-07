#include "ZombieFightStateMaasLiam.h"

#include "ZombieCombatHelperMaasLiam.h"
#include "ZombieFleeStateMaasLiam.h"
#include "ZombieMovementHelperMaasLiam.h"
#include "ZombieThreatHelperMaasLiam.h"

void FZombieFightStateMaasLiam::Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, UActorComponent* InventoryComponent, float& TimeSinceLastWeaponUse, float WeaponUseInterval, float FleeDistance)
{
	TimeSinceLastWeaponUse += Owner ? Owner->GetWorld()->GetDeltaSeconds() : 0.f;

	AActor* ClosestZombie = FZombieThreatHelperMaasLiam::GetClosestZombie(Perceptor, Owner);

	if (!ClosestZombie)
	{
		return;
	}

	FZombieMovementHelperMaasLiam::FaceActor(Owner, ClosestZombie);

	if (TimeSinceLastWeaponUse >= WeaponUseInterval)
	{
		TimeSinceLastWeaponUse = 0.f;

		if (FZombieCombatHelperMaasLiam::TryUseWeapon(InventoryComponent))
		{
			return;
		}
	}

	FZombieFleeStateMaasLiam::Execute(Owner, Perceptor, FleeDistance);
}