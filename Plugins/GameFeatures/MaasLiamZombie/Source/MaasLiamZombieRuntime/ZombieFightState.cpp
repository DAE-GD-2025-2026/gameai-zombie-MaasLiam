#include "ZombieFightState.h"

#include "ZombieCombatHelper.h"
#include "ZombieFleeState.h"
#include "ZombieMovementHelper.h"
#include "ZombieThreatHelper.h"

void FZombieFightState::Execute(AActor* Owner, UStudentPerceptor* Perceptor, UActorComponent* InventoryComponent, float& TimeSinceLastWeaponUse, float WeaponUseInterval, float FleeDistance)
{
	TimeSinceLastWeaponUse += Owner ? Owner->GetWorld()->GetDeltaSeconds() : 0.f;

	AActor* ClosestZombie = FZombieThreatHelper::GetClosestZombie(Perceptor, Owner);

	if (!ClosestZombie)
	{
		return;
	}

	FZombieMovementHelper::FaceActor(Owner, ClosestZombie);

	if (TimeSinceLastWeaponUse >= WeaponUseInterval)
	{
		TimeSinceLastWeaponUse = 0.f;

		if (FZombieCombatHelper::TryUseWeapon(InventoryComponent))
		{
			return;
		}
	}

	FZombieFleeState::Execute(Owner, Perceptor, FleeDistance);
}