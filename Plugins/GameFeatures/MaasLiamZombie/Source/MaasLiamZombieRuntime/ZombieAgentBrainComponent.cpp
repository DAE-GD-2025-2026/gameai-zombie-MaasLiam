#include "ZombieAgentBrainComponent.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "StudentPerceptor.h"

UZombieAgentBrainComponent::UZombieAgentBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UZombieAgentBrainComponent::BeginPlay()
{
	Super::BeginPlay();

	Perceptor = GetOwner()->FindComponentByClass<UStudentPerceptor>();
	InventoryComponent = FindInventoryComponent();
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
	AActor* ClosestZombie = GetClosestZombie();

	if (ClosestZombie)
	{
		const float ZombieDistance = FVector::Dist(
			GetOwner()->GetActorLocation(),
			ClosestZombie->GetActorLocation()
		);

		if (CurrentState == EZombieAgentState::Flee)
		{
			if (ZombieDistance <= ZombieDangerExitRange)
			{
				CurrentState = EZombieAgentState::Flee;
				return;
			}
		}
		else if (ZombieDistance <= ZombieDangerEnterRange)
		{
			CurrentState = EZombieAgentState::Flee;
			return;
		}
	}

	if (Perceptor && Perceptor->SeenItems.Num() > 0)
	{
		CurrentState = EZombieAgentState::SeekItem;
		return;
	}

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
		ExecuteSeekItem();
		break;
		
	case EZombieAgentState::Flee:
		ExecuteFlee();
		break;

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
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	AIController->MoveToLocation(
		GetRandomExploreLocation(),
		100.f,
		true,
		true,
		true,
		false,
		nullptr,
		true
	);
}

void UZombieAgentBrainComponent::ExecuteSeekItem()
{
	AActor* ClosestItem = GetClosestItem();
	if (!ClosestItem) return;

	if (TryPickupItem(ClosestItem))
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	AIController->MoveToActor(
		ClosestItem,
		25.f,
		true,
		true,
		true,
		nullptr,
		true
	);
}

void UZombieAgentBrainComponent::ExecuteFlee()
{
	AActor* ClosestZombie = GetClosestZombie();

	if (!ClosestZombie)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	const FVector FleeTarget = GetFleeLocation(ClosestZombie);

	AIController->MoveToLocation(
		FleeTarget,
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
		0.f,
		FColor::Red,
		TEXT("FLEEING FROM ZOMBIE")
	);
}

AActor* UZombieAgentBrainComponent::GetClosestItem() const
{
	if (!Perceptor) return nullptr;

	AActor* ClosestItem = nullptr;
	float ClosestDistance = FLT_MAX;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (AActor* Item : Perceptor->SeenItems)
	{
		if (!IsValid(Item)) continue;

		const float Distance = FVector::Dist(OwnerLocation, Item->GetActorLocation());

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestItem = Item;
		}
	}

	return ClosestItem;
}

AActor* UZombieAgentBrainComponent::GetClosestZombie() const
{
	if (!Perceptor)
	{
		return nullptr;
	}

	AActor* ClosestZombie = nullptr;
	float ClosestDistance = FLT_MAX;

	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (AActor* Zombie : Perceptor->SeenZombies)
	{
		if (!IsValid(Zombie))
		{
			continue;
		}

		const float Distance = FVector::Dist(
			OwnerLocation,
			Zombie->GetActorLocation()
		);

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestZombie = Zombie;
		}
	}

	return ClosestZombie;
}

UActorComponent* UZombieAgentBrainComponent::FindInventoryComponent() const
{
	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(Components);

	for (UActorComponent* Component : Components)
	{
		if (Component && Component->GetName().Contains(TEXT("Inventory")))
		{
			return Component;
		}
	}

	return nullptr;
}

bool UZombieAgentBrainComponent::TryPickupItem(AActor* ItemActor)
{
	if (!InventoryComponent || !ItemActor) return false;

	const float DistanceToItem = FVector::Dist(
		GetOwner()->GetActorLocation(),
		ItemActor->GetActorLocation()	
	);

	const float AllowedPickupDistance = GetPickupRange() + 75.f;

	if (DistanceToItem > AllowedPickupDistance)
	{
		return false;
	}

	const int32 Capacity = GetInventoryCapacity();

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (TryGrabItemInSlot(SlotIndex, ItemActor))
		{
			if (Perceptor)
			{
				Perceptor->SeenItems.Remove(ItemActor);
			}

			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Green,
				FString::Printf(TEXT("Picked up item in slot %d"), SlotIndex)
			);

			return true;
		}
	}

	return false;
}

bool UZombieAgentBrainComponent::TryGrabItemInSlot(int32 SlotIndex, AActor* ItemActor)
{
	if (!InventoryComponent) return false;

	UFunction* GrabFunction = InventoryComponent->FindFunction(TEXT("GrabItem"));
	if (!GrabFunction) return false;

	struct FGrabItemParams
	{
		int32 SlotIdx;
		AActor* Item;
		bool ReturnValue;
	};

	FGrabItemParams Params;
	Params.SlotIdx = SlotIndex;
	Params.Item = ItemActor;
	Params.ReturnValue = false;

	InventoryComponent->ProcessEvent(GrabFunction, &Params);

	return Params.ReturnValue;
}

float UZombieAgentBrainComponent::GetPickupRange() const
{
	if (!InventoryComponent) return 100.f;

	UFunction* RangeFunction = InventoryComponent->FindFunction(TEXT("GetPickupRange"));
	if (!RangeFunction) return 100.f;

	struct FPickupRangeParams
	{
		float ReturnValue;
	};

	FPickupRangeParams Params;
	Params.ReturnValue = 100.f;

	InventoryComponent->ProcessEvent(RangeFunction, &Params);

	return Params.ReturnValue;
}

int32 UZombieAgentBrainComponent::GetInventoryCapacity() const
{
	if (!InventoryComponent) return 5;

	UFunction* CapacityFunction = InventoryComponent->FindFunction(TEXT("GetInventoryCapacity"));
	if (!CapacityFunction) return 5;

	struct FCapacityParams
	{
		int32 ReturnValue;
	};

	FCapacityParams Params;
	Params.ReturnValue = 5;

	InventoryComponent->ProcessEvent(CapacityFunction, &Params);

	return Params.ReturnValue;
}

FVector UZombieAgentBrainComponent::GetRandomExploreLocation() const
{
	const FVector CurrentLocation = GetOwner()->GetActorLocation();

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

FVector UZombieAgentBrainComponent::GetFleeLocation(AActor* ZombieActor) const
{
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	FVector BestLocation = OwnerLocation;
	float BestScore = -FLT_MAX;

	const int32 DirectionCount = 16;

	for (int32 DirectionIndex = 0; DirectionIndex < DirectionCount; ++DirectionIndex)
	{
		const float Angle = (2.f * PI / DirectionCount) * DirectionIndex;

		const FVector Direction = FVector(
			FMath::Cos(Angle),
			FMath::Sin(Angle),
			0.f
		);

		const FVector CandidateLocation = OwnerLocation + Direction * FleeDistance;
		const float CandidateScore = ScoreFleeLocation(CandidateLocation);

		if (CandidateScore > BestScore)
		{
			BestScore = CandidateScore;
			BestLocation = CandidateLocation;
		}
	}

	return BestLocation;
}

float UZombieAgentBrainComponent::ScoreFleeLocation(const FVector& Location) const
{
	if (!Perceptor)
	{
		return 0.f;
	}

	float Score = 0.f;

	for (AActor* Zombie : Perceptor->SeenZombies)
	{
		if (!IsValid(Zombie))
		{
			continue;
		}

		const float DistanceToZombie = FVector::Dist(
			Location,
			Zombie->GetActorLocation()
		);

		Score += DistanceToZombie;
	}

	return Score;
}