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
	FColor DebugColor = FColor::White;
	FString TypeString = "Unknown";

	//Zombies
	if (ActorName.Contains("Zombie"))
	{
		TypeString = "Zombie";
		DebugColor = FColor::Red;
	}
	//Items
	else if (
		ActorName.Contains("Food") ||
		ActorName.Contains("Medkit") ||
		ActorName.Contains("Pistol") ||
		ActorName.Contains("Shotgun") ||
		ActorName.Contains("Garbage"))
	{
		TypeString = "Item";
		DebugColor = FColor::Green;
	}
	//Houses
	else if (ActorName.Contains("House"))
	{
		TypeString = "House";
		DebugColor = FColor::Blue;
	}
	//Purge zones
	else if (ActorName.Contains("Purge"))
	{
		TypeString = "PurgeZone";
		DebugColor = FColor::Purple;
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
}
