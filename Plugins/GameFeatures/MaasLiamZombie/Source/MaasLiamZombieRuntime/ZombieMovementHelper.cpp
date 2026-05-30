#include "ZombieMovementHelper.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"

bool FZombieMovementHelper::MoveToLocation(AActor* Owner, const FVector& TargetLocation, float AcceptanceRadius)
{
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn) return false;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return false;

	AIController->MoveToLocation(
		TargetLocation,
		AcceptanceRadius,
		true,
		true,
		true,
		false,
		nullptr,
		true
	);

	return true;
}

bool FZombieMovementHelper::MoveToActor(AActor* Owner, AActor* TargetActor, float AcceptanceRadius)
{
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn || !TargetActor) return false;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return false;

	AIController->MoveToActor(
		TargetActor,
		AcceptanceRadius,
		true,
		true,
		true,
		nullptr,
		true
	);

	return true;
}

bool FZombieMovementHelper::FaceActor(AActor* Owner, AActor* TargetActor)
{
	if (!Owner || !TargetActor) return false;

	const FVector DirectionToTarget = (TargetActor->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
	Owner->SetActorRotation(DirectionToTarget.Rotation());

	return true;
}