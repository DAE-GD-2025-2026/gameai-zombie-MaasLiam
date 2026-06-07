#include "ZombieSeekRememberedItemState.h"

#include "ZombieMovementHelper.h"

void FZombieSeekRememberedItemState::Execute(
	AActor* Owner,
	UStudentPerceptor* Perceptor,
	TArray<FRememberedItem>& RememberedItems,
	UActorComponent* InventoryComponent,
	UActorComponent* HealthComponent,
	UActorComponent* StaminaComponent,
	int LowHealthThreshold,
	float LowStaminaThreshold)
{
	if (!Owner)
	{
		return;
	}

	FRememberedItem* TargetMemory = FZombieItemMemoryHelper::GetBestRememberedItem(RememberedItems, Owner, InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold);

	if (!TargetMemory)
	{
		return;
	}

	const float DistanceToMemory = FVector::Dist(Owner->GetActorLocation(), TargetMemory->LastKnownLocation);

	if (DistanceToMemory <= 250.f)
	{
		FZombieItemMemoryHelper::ForgetItemNearLocation(RememberedItems, TargetMemory->LastKnownLocation, 300.f);

		return;
	}

	FZombieMovementHelper::MoveToLocation(Owner, TargetMemory->LastKnownLocation, 100.f);
}