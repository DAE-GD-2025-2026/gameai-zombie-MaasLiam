#include "ZombieSeekRememberedItemStateMaasLiam.h"

#include "ZombieMovementHelperMaasLiam.h"

void FZombieSeekRememberedItemStateMaasLiam::Execute(
	AActor* Owner,
	UStudentPerceptorMaasLiam* Perceptor,
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

	FRememberedItem* TargetMemory = FZombieItemMemoryHelperMaasLiam::GetBestRememberedItem(RememberedItems, Owner, InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold);

	if (!TargetMemory)
	{
		return;
	}

	const float DistanceToMemory = FVector::Dist(Owner->GetActorLocation(), TargetMemory->LastKnownLocation);

	if (DistanceToMemory <= 250.f)
	{
		FZombieItemMemoryHelperMaasLiam::ForgetItemNearLocation(RememberedItems, TargetMemory->LastKnownLocation, 300.f);

		return;
	}

	FZombieMovementHelperMaasLiam::MoveToLocation(Owner, TargetMemory->LastKnownLocation, 100.f);
}