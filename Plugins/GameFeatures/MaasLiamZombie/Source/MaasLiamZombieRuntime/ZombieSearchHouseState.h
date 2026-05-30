#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

class FZombieSearchHouseState
{
public:
	static void Execute(AActor* Owner, UStudentPerceptor* Perceptor, TArray<AActor*>& SearchedHouses, float HouseSearchAcceptanceRadius);
};