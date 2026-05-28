#include "ZombieAgentBrainComponent.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"

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
	ExecuteCurrentState(DeltaTime);

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

void UZombieAgentBrainComponent::ExecuteCurrentState(float DeltaTime)
{
	switch (CurrentState)
	{
	case EZombieAgentState::Explore:
		ExecuteExplore(DeltaTime);
		break;

	case EZombieAgentState::SeekItem:
	case EZombieAgentState::Flee:
	case EZombieAgentState::Fight:
	case EZombieAgentState::UseItem:
	default:
		break;
	}
}

void UZombieAgentBrainComponent::ExecuteExplore(float DeltaTime)
{
	TimeSinceLastExploreMove += DeltaTime;

	if (TimeSinceLastExploreMove < ExploreMoveInterval)
	{
		return;
	}

	TimeSinceLastExploreMove = 0.f;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController)
	{
		return;
	}

	const FVector TargetLocation = GetRandomExploreLocation();

	AIController->MoveToLocation(
		TargetLocation,
		100.f,
		true,
		true,
		true,
		false,
		nullptr,
		true
	);

	GEngine->AddOnScreenDebugMessage(
		-1,
		2.f,
		FColor::Green,
		TEXT("Explore: moving to random location")
	);
}

FVector UZombieAgentBrainComponent::GetRandomExploreLocation() const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return FVector::ZeroVector;
	}

	const FVector CurrentLocation = OwnerActor->GetActorLocation();

	const FVector RandomDirection = FVector(
		FMath::FRandRange(-1.f, 1.f),
		FMath::FRandRange(-1.f, 1.f),
		0.f
	).GetSafeNormal();

	return CurrentLocation + RandomDirection * ExploreRadius;
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