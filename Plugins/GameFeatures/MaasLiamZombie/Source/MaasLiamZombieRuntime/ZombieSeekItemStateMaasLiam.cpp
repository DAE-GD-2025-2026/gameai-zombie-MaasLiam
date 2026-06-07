#include "ZombieSeekItemStateMaasLiam.h"

#include "StudentPerceptorMaasLiam.h"
#include "ZombieInventoryHelperMaasLiam.h"
#include "ZombieMovementHelperMaasLiam.h"

void FZombieSeekItemStateMaasLiam::Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, TArray<FRememberedItem>& RememberedItems, UActorComponent* InventoryComponent)
{
	AActor* ClosestItem = FZombieInventoryHelperMaasLiam::GetBestItem(Perceptor, Owner);

	if (!ClosestItem)
	{
		return;
	}

	if (FZombieInventoryHelperMaasLiam::IsInventoryFull(InventoryComponent))
	{
		if (FZombieInventoryHelperMaasLiam::TryReplaceInventoryItem(InventoryComponent, Perceptor, ClosestItem))
		{
			FZombieItemMemoryHelperMaasLiam::ForgetItem(RememberedItems, ClosestItem);
			return;
		}

		if (Perceptor)
		{
			Perceptor->SeenItems.Remove(ClosestItem);
		}

		return;
	}

	if (FZombieInventoryHelperMaasLiam::TryPickupItem(Owner, InventoryComponent, Perceptor, ClosestItem))
	{
		FZombieItemMemoryHelperMaasLiam::ForgetItem(RememberedItems, ClosestItem);
		return;
	}

	FZombieMovementHelperMaasLiam::MoveToActor(Owner, ClosestItem, 25.f);
}