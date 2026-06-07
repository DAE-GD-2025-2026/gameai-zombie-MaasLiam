#pragma once

#include "CoreMinimal.h"

class UStudentPerceptorMaasLiam;

class FZombieSearchHouseStateMaasLiam
{
public:
	static void Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, TArray<AActor*>& SearchedHouses, float HouseSearchAcceptanceRadius);
};