#pragma once

#include "CoreMinimal.h"

class UStudentPerceptorMaasLiam;

class FZombieFleeStateMaasLiam
{
public:
	static void Execute(AActor* Owner, UStudentPerceptorMaasLiam* Perceptor, float FleeDistance);
};