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

void UStudentPerceptor::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateZombieMemory(DeltaTime);
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;

	const FString ActorName = Actor->GetName();
	const FString ClassName = Actor->GetClass()->GetName();

	if (!Stimulus.WasSuccessfullySensed())
	{
		SeenItems.Remove(Actor);
		SeenHouses.Remove(Actor);
		SeenPurgeZones.Remove(Actor);

		return;
	}

	FColor DebugColor = FColor::White;
	FString TypeString = "Unknown";

	if (IsZombieActor(Actor))
	{
		TypeString = "Zombie";
		DebugColor = FColor::Red;

		RememberZombie(Actor);
	}
	else if (
		ActorName.Contains(TEXT("Food")) ||
		ActorName.Contains(TEXT("Medkit")) ||
		ActorName.Contains(TEXT("Pistol")) ||
		ActorName.Contains(TEXT("Shotgun")) ||
		ActorName.Contains(TEXT("Garbage")) ||
		ClassName.Contains(TEXT("Food")) ||
		ClassName.Contains(TEXT("Medkit")) ||
		ClassName.Contains(TEXT("Pistol")) ||
		ClassName.Contains(TEXT("Shotgun")) ||
		ClassName.Contains(TEXT("Garbage")))
	{
		TypeString = "Item";
		DebugColor = FColor::Green;

		if (!SeenItems.Contains(Actor))
		{
			SeenItems.Add(Actor);
		}
	}
	else if (ActorName.Contains(TEXT("House")) || ClassName.Contains(TEXT("House")))
	{
		TypeString = "House";
		DebugColor = FColor::Blue;

		if (!SeenHouses.Contains(Actor))
		{
			SeenHouses.Add(Actor);
		}
	}
	else if (ActorName.Contains(TEXT("Purge")) || ClassName.Contains(TEXT("Purge")))
	{
		TypeString = "PurgeZone";
		DebugColor = FColor::Purple;

		if (!SeenPurgeZones.Contains(Actor))
		{
			SeenPurgeZones.Add(Actor);
		}
	}

	GEngine->AddOnScreenDebugMessage(
		-1,
		1.f,
		DebugColor,
		FString::Printf(TEXT("Detected %s : %s"), *TypeString, *ActorName)
	);
}

bool UStudentPerceptor::IsZombieActor(AActor* Actor) const
{
	if (!Actor) return false;

	const FString ActorName = Actor->GetName();
	const FString ClassName = Actor->GetClass()->GetName();

	return
		ActorName.Contains(TEXT("Zombie")) ||
		ActorName.Contains(TEXT("Runner")) ||
		ActorName.Contains(TEXT("Heavy")) ||
		ActorName.Contains(TEXT("Normal")) ||
		ClassName.Contains(TEXT("Zombie")) ||
		ClassName.Contains(TEXT("Runner")) ||
		ClassName.Contains(TEXT("Heavy")) ||
		ClassName.Contains(TEXT("Normal"));
}

void UStudentPerceptor::RememberZombie(AActor* Zombie)
{
	if (!Zombie) return;

	if (!SeenZombies.Contains(Zombie))
	{
		SeenZombies.Add(Zombie);
	}

	for (FZombieThreatMemory& Memory : ZombieMemories)
	{
		if (Memory.Zombie == Zombie)
		{
			Memory.TimeSinceLastSeen = 0.f;
			return;
		}
	}

	FZombieThreatMemory NewMemory;
	NewMemory.Zombie = Zombie;
	NewMemory.TimeSinceLastSeen = 0.f;
	ZombieMemories.Add(NewMemory);
}

void UStudentPerceptor::UpdateZombieMemory(float DeltaTime)
{
	for (int32 Index = ZombieMemories.Num() - 1; Index >= 0; --Index)
	{
		FZombieThreatMemory& Memory = ZombieMemories[Index];

		if (!IsValid(Memory.Zombie))
		{
			ZombieMemories.RemoveAt(Index);
			continue;
		}

		Memory.TimeSinceLastSeen += DeltaTime;

		if (Memory.TimeSinceLastSeen > ZombieMemoryDuration)
		{
			SeenZombies.Remove(Memory.Zombie);
			ZombieMemories.RemoveAt(Index);
		}
	}
}