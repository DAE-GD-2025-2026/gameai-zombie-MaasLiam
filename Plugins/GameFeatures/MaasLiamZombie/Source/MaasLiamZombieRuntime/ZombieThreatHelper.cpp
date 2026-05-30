#include "ZombieThreatHelper.h"
#include "StudentPerceptor.h"

AActor* FZombieThreatHelper::GetClosestZombie(UStudentPerceptor* Perceptor, AActor* Owner)
{
	if (!Perceptor || !Owner) return nullptr;

	AActor* ClosestZombie = nullptr;
	float ClosestDistance = FLT_MAX;

	const FVector OwnerLocation = Owner->GetActorLocation();

	for (AActor* Zombie : Perceptor->SeenZombies)
	{
		if (!IsValid(Zombie)) continue;

		const float Distance = FVector::Dist(OwnerLocation, Zombie->GetActorLocation());

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestZombie = Zombie;
		}
	}

	return ClosestZombie;
}

AActor* FZombieThreatHelper::GetClosestPurgeZone(UStudentPerceptor* Perceptor, AActor* Owner)
{
	if (!Perceptor || !Owner) return nullptr;

	AActor* ClosestPurgeZone = nullptr;
	float ClosestDistance = FLT_MAX;

	const FVector OwnerLocation = Owner->GetActorLocation();

	for (AActor* PurgeZone : Perceptor->SeenPurgeZones)
	{
		if (!IsValid(PurgeZone)) continue;

		const float Distance = FVector::Dist(OwnerLocation, PurgeZone->GetActorLocation());

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPurgeZone = PurgeZone;
		}
	}

	return ClosestPurgeZone;
}

FVector FZombieThreatHelper::GetFleeLocation(UStudentPerceptor* Perceptor, AActor* Owner, float FleeDistance)
{
	if (!Owner)
	{
		return FVector::ZeroVector;
	}

	const FVector OwnerLocation = Owner->GetActorLocation();

	FVector BestLocation = OwnerLocation;
	float BestScore = -FLT_MAX;

	const int32 DirectionCount = 16;

	for (int32 DirectionIndex = 0; DirectionIndex < DirectionCount; ++DirectionIndex)
	{
		const float Angle = (2.f * PI / DirectionCount) * DirectionIndex;
		const FVector Direction = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f);
		const FVector CandidateLocation = OwnerLocation + Direction * FleeDistance;
		const float CandidateScore = ScoreFleeLocation(Perceptor, CandidateLocation);

		if (CandidateScore > BestScore)
		{
			BestScore = CandidateScore;
			BestLocation = CandidateLocation;
		}
	}

	return BestLocation;
}

FVector FZombieThreatHelper::GetPurgeAvoidanceLocation(AActor* Owner, AActor* PurgeZone, float PurgeFleeDistance)
{
	if (!Owner || !PurgeZone)
	{
		return Owner ? Owner->GetActorLocation() : FVector::ZeroVector;
	}

	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector AwayDirection = (OwnerLocation - PurgeZone->GetActorLocation()).GetSafeNormal();

	return OwnerLocation + AwayDirection * PurgeFleeDistance;
}

float FZombieThreatHelper::ScoreFleeLocation(UStudentPerceptor* Perceptor, const FVector& Location)
{
	if (!Perceptor) return 0.f;

	float Score = 0.f;

	for (AActor* Zombie : Perceptor->SeenZombies)
	{
		if (!IsValid(Zombie)) continue;

		const float DistanceToZombie = FVector::Dist(Location, Zombie->GetActorLocation());
		Score += DistanceToZombie;
	}

	for (AActor* PurgeZone : Perceptor->SeenPurgeZones)
	{
		if (!IsValid(PurgeZone)) continue;
		
		const float DistanceToPurge = FVector::Dist(Location, PurgeZone->GetActorLocation());
		Score += DistanceToPurge * 0.5f;
	}

	Score += FMath::FRandRange(0.f, 100.f);

	return Score;
}