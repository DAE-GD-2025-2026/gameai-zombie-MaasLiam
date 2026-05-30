#pragma once

#include "CoreMinimal.h"
#include "ZombieAgentBrainComponent.h"

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
		float ZombieFightRange,
		float ZombieDangerEnterRange,
		float ZombieDangerExitRange,
		float PurgeDangerRange,
		int LowHealthThreshold,
		float LowStaminaThreshold
	);
};