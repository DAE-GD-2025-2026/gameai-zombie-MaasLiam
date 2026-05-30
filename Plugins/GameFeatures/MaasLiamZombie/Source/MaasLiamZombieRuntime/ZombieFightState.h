#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

class FZombieFightState
{
public:
	static void Execute(AActor* Owner, UStudentPerceptor* Perceptor, UActorComponent* InventoryComponent, float& TimeSinceLastWeaponUse, float WeaponUseInterval, float FleeDistance);
};