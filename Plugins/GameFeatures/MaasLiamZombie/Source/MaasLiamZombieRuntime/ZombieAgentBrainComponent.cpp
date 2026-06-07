#include "ZombieAgentBrainComponent.h"

#include "StudentPerceptor.h"
#include "ZombieThreatHelper.h"
#include "ZombieSurvivorStatusHelper.h"
#include "ZombieMovementHelper.h"
#include "ZombieStateSelector.h"
#include "ZombieExploreState.h"
#include "ZombieSeekItemState.h"
#include "ZombieFleeState.h"
#include "ZombieFightState.h"
#include "ZombieSearchHouseState.h"
#include  "ZombieSprintHelper.h"
#include "ZombieSeekRememberedItemState.h"

UZombieAgentBrainComponent::UZombieAgentBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UZombieAgentBrainComponent::BeginPlay()
{
	Super::BeginPlay();

	Perceptor = GetOwner()->FindComponentByClass<UStudentPerceptor>();
	InventoryComponent = FZombieSurvivorStatusHelper::FindComponentByNamePart(GetOwner(), TEXT("Inventory"));
	HealthComponent = FZombieSurvivorStatusHelper::FindComponentByNamePart(GetOwner(), TEXT("Health"));
	StaminaComponent = FZombieSurvivorStatusHelper::FindComponentByNamePart(GetOwner(), TEXT("Stamina"));
}

void UZombieAgentBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (VillageSweepTimeRemaining > 0.f)
	{
		VillageSweepTimeRemaining -= DeltaTime;
		VillageSweepTargetTimeRemaining -= DeltaTime;
	}
	
	FZombieItemMemoryHelper::UpdateItemMemory(RememberedItems, Perceptor, DeltaTime, ItemMemoryDuration);
	UpdateState();
	ExecuteCurrentState(DeltaTime);
	
	const bool bShouldSprint = CurrentState == EZombieAgentState::Flee || CurrentState == EZombieAgentState::AvoidPurge;

	FZombieSprintHelper::UpdateSprint(GetOwner(), StaminaComponent, bShouldSprint, MinimumSprintStamina);

	GEngine->AddOnScreenDebugMessage(
		20,
		0.f,
		FColor::Cyan,
		FString::Printf(TEXT("AI State: %s"), *GetStateName())
	);
}

void UZombieAgentBrainComponent::UpdateState()
{
	if (InitialScanTimeRemaining > 0.f)
	{
		CurrentState = EZombieAgentState::InitialScan;
		return;
	}
	
	CurrentState = FZombieStateSelector::SelectState(CurrentState, GetOwner(), Perceptor, InventoryComponent,
		HealthComponent, StaminaComponent, SearchedHouses, RememberedItems, ZombieFightRange,
		ZombieDangerEnterRange, ZombieDangerExitRange, PurgeDangerRange, LowHealthThreshold, LowStaminaThreshold);
	
	if (CurrentState == EZombieAgentState::SeekItem)
	{
		VillageSweepTimeRemaining = 0.f;
	}
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

	case EZombieAgentState::UseItem:
		ExecuteUseItem();
		break;
		
	case EZombieAgentState::Fight:
		ExecuteFight(DeltaTime);
		break;
		
	case EZombieAgentState::SearchHouse:
		ExecuteSearchHouse();
		break;
		
	case EZombieAgentState::AvoidPurge:
		ExecuteAvoidPurge();
		break;
		
	case EZombieAgentState::SeekRememberedItem:
		ExecuteSeekRememberedItem();
		break;
		
	case EZombieAgentState::InitialScan:
		ExecuteInitialScan(DeltaTime);
		break;

	default:
		break;
	}
}

void UZombieAgentBrainComponent::ExecuteExplore(float DeltaTime)
{
	if (VillageSweepTimeRemaining > 0.f)
	{
		const float DistanceToSweepTarget = FVector::Dist(GetOwner()->GetActorLocation(), CurrentVillageSweepTarget);

		if (DistanceToSweepTarget <= 150.f || VillageSweepTargetTimeRemaining <= 0.f)
		{
			CurrentVillageSweepTarget = GetVillageSweepLocation();
			VillageSweepTargetTimeRemaining = VillageSweepTargetDuration;
		}

		FZombieMovementHelper::MoveToLocation(GetOwner(), CurrentVillageSweepTarget, 100.f);

		return;
	}

	FZombieExploreState::Execute(GetOwner(), TimeSinceLastExploreMove, ExploreMoveInterval, ExploreRadius,
	RecentlyExploredLocations, MaxRecentExploreLocations, ExploreCandidateCount);
}

void UZombieAgentBrainComponent::ExecuteSeekItem()
{
	FZombieSeekItemState::Execute(GetOwner(), Perceptor, RememberedItems, InventoryComponent);
}

void UZombieAgentBrainComponent::ExecuteFlee()
{
	FZombieFleeState::Execute(GetOwner(), Perceptor, FleeDistance);
}

void UZombieAgentBrainComponent::ExecuteUseItem()
{
	if (FZombieSurvivorStatusHelper::TryUseInventoryItem(InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Used inventory item"));
	}
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
	case EZombieAgentState::SearchHouse:
		return "SearchHouse";
	case EZombieAgentState::AvoidPurge:
		return "AvoidPurge";
	case EZombieAgentState::InitialScan:
		return "InitialScan";
	default:
		return "Unknown";
	}
}

void UZombieAgentBrainComponent::ExecuteFight(float DeltaTime)
{
	FZombieFightState::Execute(GetOwner(), Perceptor, InventoryComponent, TimeSinceLastWeaponUse, WeaponUseInterval, FleeDistance);
}

void UZombieAgentBrainComponent::ExecuteSearchHouse()
{
	FZombieSearchHouseState::Execute(GetOwner(), Perceptor, SearchedHouses, HouseSearchAcceptanceRadius);
}

void UZombieAgentBrainComponent::ExecuteAvoidPurge()
{
	AActor* ClosestPurgeZone = FZombieThreatHelper::GetClosestPurgeZone(Perceptor, GetOwner());
	if (!ClosestPurgeZone) return;

	FZombieMovementHelper::MoveToLocation(GetOwner(), FZombieThreatHelper::GetPurgeAvoidanceLocation(GetOwner(), ClosestPurgeZone, PurgeFleeDistance), 100.f);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Avoiding purge zone"));
}

FVector UZombieAgentBrainComponent::GetVillageSweepLocation() const
{
	const FVector RandomDirection = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal();
	return VillageSweepLocation + RandomDirection * VillageSweepRadius;
}

void UZombieAgentBrainComponent::StartVillageSweep(const FVector& Location)
{
	VillageSweepLocation = Location;
	VillageSweepTimeRemaining = VillageSweepDuration;

	CurrentVillageSweepTarget = GetVillageSweepLocation();
	VillageSweepTargetTimeRemaining = VillageSweepTargetDuration;
}

void UZombieAgentBrainComponent::ExecuteSeekRememberedItem()
{
	FZombieSeekRememberedItemState::Execute(GetOwner(), Perceptor, RememberedItems, InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold);
}

void UZombieAgentBrainComponent::ExecuteInitialScan(float DeltaTime)
{
	if (!GetOwner())
	{
		return;
	}

	InitialScanTimeRemaining -= DeltaTime;
	const FRotator CurrentRotation = GetOwner()->GetActorRotation();
	const FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + InitialScanRotationSpeed * DeltaTime, CurrentRotation.Roll);
	GetOwner()->SetActorRotation(NewRotation);
}