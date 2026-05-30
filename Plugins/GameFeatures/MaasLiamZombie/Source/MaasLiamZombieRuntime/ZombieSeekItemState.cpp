#include "ZombieSeekItemState.h"

#include "StudentPerceptor.h"
#include "ZombieInventoryHelper.h"
#include "ZombieMovementHelper.h"

void FZombieSeekItemState::Execute(AActor* Owner, UStudentPerceptor* Perceptor, UActorComponent* InventoryComponent)
{
	AActor* ClosestItem = FZombieInventoryHelper::GetBestItem(Perceptor, Owner);

	if (!ClosestItem)
	{
		return;
	}

	if (FZombieInventoryHelper::IsInventoryFull(InventoryComponent))
	{
		if (FZombieInventoryHelper::TryReplaceInventoryItem(InventoryComponent, Perceptor, ClosestItem))
		{
			return;
		}

		if (Perceptor)
		{
			Perceptor->SeenItems.Remove(ClosestItem);
		}

		return;
	}

	if (FZombieInventoryHelper::TryPickupItem(Owner, InventoryComponent, Perceptor, ClosestItem))
	{
		return;
	}

	FZombieMovementHelper::MoveToActor(Owner, ClosestItem, 25.f);
}