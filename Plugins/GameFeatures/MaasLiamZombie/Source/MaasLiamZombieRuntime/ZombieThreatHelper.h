#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

class FZombieThreatHelper
{
public:
	static AActor* GetClosestZombie(UStudentPerceptor* Perceptor, AActor* Owner);
	static AActor* GetClosestPurgeZone(UStudentPerceptor* Perceptor, AActor* Owner);

	static FVector GetFleeLocation(UStudentPerceptor* Perceptor, AActor* Owner, float FleeDistance);
	static FVector GetPurgeAvoidanceLocation(AActor* Owner, AActor* PurgeZone, float PurgeFleeDistance);

private:
	static float ScoreFleeLocation(UStudentPerceptor* Perceptor, const FVector& Location);
};