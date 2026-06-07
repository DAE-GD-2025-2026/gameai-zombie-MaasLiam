#pragma once

#include "CoreMinimal.h"
#include "ZombieItemMemoryHelperMaasLiam.h"

class UStudentPerceptorMaasLiam;

class FZombieSeekRememberedItemStateMaasLiam
{
public:
	static void Execute(
		AActor* Owner,
		UStudentPerceptorMaasLiam* Perceptor,
		TArray<FRememberedItem>& RememberedItems,
		UActorComponent* InventoryComponent,
		UActorComponent* HealthComponent,
		UActorComponent* StaminaComponent,
		int LowHealthThreshold,
		float LowStaminaThreshold
	);
};