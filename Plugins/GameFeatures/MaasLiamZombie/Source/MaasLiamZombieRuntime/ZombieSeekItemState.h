#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

class FZombieSeekItemState
{
public:
	static void Execute(AActor* Owner, UStudentPerceptor* Perceptor, UActorComponent* InventoryComponent);
};