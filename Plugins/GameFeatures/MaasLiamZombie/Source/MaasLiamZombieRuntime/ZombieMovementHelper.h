#pragma once

#include "CoreMinimal.h"

class FZombieMovementHelper
{
public:
	static bool MoveToLocation(AActor* Owner, const FVector& TargetLocation, float AcceptanceRadius);
	static bool MoveToActor(AActor* Owner, AActor* TargetActor, float AcceptanceRadius);
	static bool FaceActor(AActor* Owner, AActor* TargetActor);
};