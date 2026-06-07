#pragma once

#include "CoreMinimal.h"
#include "ZombieAgentBrainComponent.h"
#include "ZombieItemMemoryHelper.h"

class UStudentPerceptor;

class FZombieStateSelector
{
public:
	static EZombieAgentState SelectState(
		EZombieAgentState CurrentState,
		AActor* Owner,
		UStudentPerceptor* Perceptor,
		UActorComponent* InventoryComponent,
		UActorComponent* HealthComponent,
		UActorComponent* StaminaComponent,
		const TArray<AActor*>& SearchedHouses,
		const TArray<FRememberedItem>& RememberedItems,
		float ZombieFightRange,
		float ZombieDangerEnterRange,
		float ZombieDangerExitRange,
		float PurgeDangerRange,
		int LowHealthThreshold,
		float LowStaminaThreshold
	);
};