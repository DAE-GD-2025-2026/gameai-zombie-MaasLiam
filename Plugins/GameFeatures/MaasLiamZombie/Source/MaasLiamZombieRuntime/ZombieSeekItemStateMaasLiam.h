#pragma once

#include "CoreMinimal.h"
#include "ZombieItemMemoryHelperMaasLiam.h"

class UStudentPerceptorMaasLiam;

class FZombieSeekItemStateMaasLiam
{
public:
	static void Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, TArray<FRememberedItem>& RememberedItems, UActorComponent* InventoryComponent);
};