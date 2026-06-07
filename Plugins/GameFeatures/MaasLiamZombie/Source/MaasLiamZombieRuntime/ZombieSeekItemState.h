#pragma once

#include "CoreMinimal.h"
#include "ZombieItemMemoryHelper.h"

class UStudentPerceptor;

class FZombieSeekItemState
{
public:
	static void Execute(AActor* Owner, UStudentPerceptor* Perceptor, TArray<FRememberedItem>& RememberedItems, UActorComponent* InventoryComponent);
};