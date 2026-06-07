#pragma once

#include "CoreMinimal.h"

class FZombieSprintHelperMaasLiam
{
public:
	static void UpdateSprint(AActor* Owner, UActorComponent* StaminaComponent, bool bShouldSprint, float MinimumSprintStamina);

private:
	static void StartRunning(AActor* Owner);
	static void StopRunning(AActor* Owner);
	static bool IsRunning(AActor* Owner);
	static float GetCurrentStamina(UActorComponent* StaminaComponent);
};