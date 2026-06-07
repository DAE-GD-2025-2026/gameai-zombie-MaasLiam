#pragma once

#include "CoreMinimal.h"
#include "ZombieAgentBrainComponentMaasLiam.h"
#include "ZombieItemMemoryHelperMaasLiam.h"

class UStudentPerceptorMaasLiam;

class FZombieStateSelectorMaasLiam
{
public:
	static EZombieAgentState SelectState(
		EZombieAgentState CurrentState,
		AActor* Owner,
		UStudentPerceptorMaasLiam* Perceptor,
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