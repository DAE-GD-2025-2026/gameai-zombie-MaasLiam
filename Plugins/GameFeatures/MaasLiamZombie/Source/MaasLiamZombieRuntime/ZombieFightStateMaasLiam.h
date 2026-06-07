#pragma once

#include "CoreMinimal.h"

class UStudentPerceptorMaasLiam;

class FZombieFightStateMaasLiam
{
public:
	static void Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, UActorComponent* InventoryComponent, float& TimeSinceLastWeaponUse, float WeaponUseInterval, float FleeDistance);
};