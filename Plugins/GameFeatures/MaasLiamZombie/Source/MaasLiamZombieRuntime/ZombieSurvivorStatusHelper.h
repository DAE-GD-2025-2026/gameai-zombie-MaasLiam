#pragma once

#include "CoreMinimal.h"

class FZombieSurvivorStatusHelper
{
public:
	static UActorComponent* FindComponentByNamePart(AActor* Owner, const FString& NamePart);

	static int GetCurrentHealth(UActorComponent* HealthComponent);
	static float GetCurrentStamina(UActorComponent* StaminaComponent);

	static bool ShouldUseItem(
		UActorComponent* InventoryComponent,
		UActorComponent* HealthComponent,
		UActorComponent* StaminaComponent,
		int LowHealthThreshold,
		float LowStaminaThreshold
	);

	static bool TryUseInventoryItem(
		UActorComponent* InventoryComponent,
		UActorComponent* HealthComponent,
		UActorComponent* StaminaComponent,
		int LowHealthThreshold,
		float LowStaminaThreshold
	);
};