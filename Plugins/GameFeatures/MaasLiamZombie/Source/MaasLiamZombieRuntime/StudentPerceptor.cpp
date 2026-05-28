// Fill out your copyright notice in the Description page of Project Settings.

#include "StudentPerceptor.h"

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();

	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	FString ActorName = Actor->GetName();

	// LOST SIGHT
	if (!Stimulus.WasSuccessfullySensed())
	{
		SeenZombies.Remove(Actor);
		SeenItems.Remove(Actor);
		SeenHouses.Remove(Actor);
		SeenPurgeZones.Remove(Actor);

		return;
	}

	FColor DebugColor = FColor::White;
	FString TypeString = "Unknown";

	// ZOMBIES
	if (ActorName.Contains("Zombie"))
	{
		TypeString = "Zombie";
		DebugColor = FColor::Red;

		if (!SeenZombies.Contains(Actor))
		{
			SeenZombies.Add(Actor);
		}
	}
	// ITEMS
	else if (
		ActorName.Contains("Food") ||
		ActorName.Contains("Medkit") ||
		ActorName.Contains("Pistol") ||
		ActorName.Contains("Shotgun") ||
		ActorName.Contains("Garbage"))
	{
		TypeString = "Item";
		DebugColor = FColor::Green;

		if (!SeenItems.Contains(Actor))
		{
			SeenItems.Add(Actor);
		}
	}
	// HOUSES
	else if (ActorName.Contains("House"))
	{
		TypeString = "House";
		DebugColor = FColor::Blue;

		if (!SeenHouses.Contains(Actor))
		{
			SeenHouses.Add(Actor);
		}
	}
	// PURGE ZONES
	else if (ActorName.Contains("Purge"))
	{
		TypeString = "PurgeZone";
		DebugColor = FColor::Purple;

		if (!SeenPurgeZones.Contains(Actor))
		{
			SeenPurgeZones.Add(Actor);
		}
	}

	FString DebugMessage = FString::Printf(
		TEXT("Detected %s : %s"),
		*TypeString,
		*ActorName
	);

	GEngine->AddOnScreenDebugMessage(
		-1,
		2.f,
		DebugColor,
		DebugMessage
	);

	// MEMORY DEBUG
	GEngine->AddOnScreenDebugMessage(
		-1,
		2.f,
		FColor::Yellow,
		FString::Printf(
			TEXT("Memory -> Zombies: %d | Items: %d | Houses: %d | Purge: %d"),
			SeenZombies.Num(),
			SeenItems.Num(),
			SeenHouses.Num(),
			SeenPurgeZones.Num()
		)
	);
}