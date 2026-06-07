#pragma once

#include "CoreMinimal.h"

class UStudentPerceptorMaasLiam;

class FZombieThreatHelperMaasLiam
{
public:
	static AActor* GetClosestZombie(UStudentPerceptorMaasLiam* Perceptor, AActor* Owner);
	static AActor* GetClosestPurgeZone(UStudentPerceptorMaasLiam* Perceptor, AActor* Owner);

	static FVector GetFleeLocation(UStudentPerceptorMaasLiam* Perceptor, AActor* Owner, float FleeDistance);
	static FVector GetPurgeAvoidanceLocation(AActor* Owner, AActor* PurgeZone, float PurgeFleeDistance);

private:
	static float ScoreFleeLocation(UStudentPerceptorMaasLiam* Perceptor, const FVector& Location);
};