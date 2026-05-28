#include "ZombieAgentBrainComponent.h"

UZombieAgentBrainComponent::UZombieAgentBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UZombieAgentBrainComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UZombieAgentBrainComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateState();

	GEngine->AddOnScreenDebugMessage(
		20,
		0.f,
		FColor::Cyan,
		FString::Printf(TEXT("AI State: %s"), *GetStateName())
	);
}

void UZombieAgentBrainComponent::UpdateState()
{
	CurrentState = EZombieAgentState::Explore;
}

FString UZombieAgentBrainComponent::GetStateName() const
{
	switch (CurrentState)
	{
	case EZombieAgentState::Explore:
		return "Explore";
	case EZombieAgentState::SeekItem:
		return "SeekItem";
	case EZombieAgentState::Flee:
		return "Flee";
	case EZombieAgentState::Fight:
		return "Fight";
	case EZombieAgentState::UseItem:
		return "UseItem";
	default:
		return "Unknown";
	}
}