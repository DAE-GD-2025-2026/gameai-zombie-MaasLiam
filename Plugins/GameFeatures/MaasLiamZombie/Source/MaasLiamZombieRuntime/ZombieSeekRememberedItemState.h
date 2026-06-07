#pragma once

#include "CoreMinimal.h"
#include "ZombieItemMemoryHelper.h"

class UStudentPerceptor;

class FZombieSeekRememberedItemState
{
public:
	static void Execute(
		AActor* Owner,
		UStudentPerceptor* Perceptor,
		TArray<FRememberedItem>& RememberedItems,
		UActorComponent* InventoryComponent,
		UActorComponent* HealthComponent,
		UActorComponent* StaminaComponent,
		int LowHealthThreshold,
		float LowStaminaThreshold
	);
};