#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

class FZombieFleeState
{
public:
	static void Execute(AActor* Owner, UStudentPerceptor* Perceptor, float FleeDistance);
};