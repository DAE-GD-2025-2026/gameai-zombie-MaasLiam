#include "ZombieAgentBrainComponentMaasLiam.h"

#include "StudentPerceptorMaasLiam.h"
#include "ZombieThreatHelperMaasLiam.h"
#include "ZombieSurvivorStatusHelperMaasLiam.h"
#include "ZombieMovementHelperMaasLiam.h"
#include "ZombieStateSelectorMaasLiam.h"
#include "ZombieExploreStateMaasLiam.h"
#include "ZombieSeekItemStateMaasLiam.h"
#include "ZombieFleeStateMaasLiam.h"
#include "ZombieFightStateMaasLiam.h"
#include "ZombieSearchHouseStateMaasLiam.h"
#include  "ZombieSprintHelperMaasLiam.h"
#include "ZombieSeekRememberedItemStateMaasLiam.h"

UZombieAgentBrainComponentMaasLiam::UZombieAgentBrainComponentMaasLiam()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UZombieAgentBrainComponentMaasLiam::BeginPlay()
{
	Super::BeginPlay();

	Perceptor = GetOwner()->FindComponentByClass<UStudentPerceptorMaasLiam>();
	InventoryComponent = FZombieSurvivorStatusHelperMaasLiam::FindComponentByNamePart(GetOwner(), TEXT("Inventory"));
	HealthComponent = FZombieSurvivorStatusHelperMaasLiam::FindComponentByNamePart(GetOwner(), TEXT("Health"));
	StaminaComponent = FZombieSurvivorStatusHelperMaasLiam::FindComponentByNamePart(GetOwner(), TEXT("Stamina"));
}

void UZombieAgentBrainComponentMaasLiam::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (VillageSweepTimeRemaining > 0.f)
	{
		VillageSweepTimeRemaining -= DeltaTime;
		VillageSweepTargetTimeRemaining -= DeltaTime;
	}
	
	FZombieItemMemoryHelperMaasLiam::UpdateItemMemory(RememberedItems, Perceptor, DeltaTime, ItemMemoryDuration);
	UpdateState();
	ExecuteCurrentState(DeltaTime);
	
	const bool bShouldSprint = CurrentState == EZombieAgentState::Flee || CurrentState == EZombieAgentState::AvoidPurge;

	FZombieSprintHelperMaasLiam::UpdateSprint(GetOwner(), StaminaComponent, bShouldSprint, MinimumSprintStamina);

	GEngine->AddOnScreenDebugMessage(
		20,
		0.f,
		FColor::Cyan,
		FString::Printf(TEXT("AI State: %s"), *GetStateName())
	);
}

void UZombieAgentBrainComponentMaasLiam::UpdateState()
{
	if (InitialScanTimeRemaining > 0.f)
	{
		CurrentState = EZombieAgentState::InitialScan;
		return;
	}
	
	CurrentState = FZombieStateSelectorMaasLiam::SelectState(CurrentState, GetOwner(), Perceptor, InventoryComponent,
		HealthComponent, StaminaComponent, SearchedHouses, RememberedItems, ZombieFightRange,
		ZombieDangerEnterRange, ZombieDangerExitRange, PurgeDangerRange, LowHealthThreshold, LowStaminaThreshold);
	
	if (CurrentState == EZombieAgentState::SeekItem)
	{
		VillageSweepTimeRemaining = 0.f;
	}
}

void UZombieAgentBrainComponentMaasLiam::ExecuteCurrentState(float DeltaTime)
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

void UZombieAgentBrainComponentMaasLiam::ExecuteExplore(float DeltaTime)
{
	if (VillageSweepTimeRemaining > 0.f)
	{
		const float DistanceToSweepTarget = FVector::Dist(GetOwner()->GetActorLocation(), CurrentVillageSweepTarget);

		if (DistanceToSweepTarget <= 150.f || VillageSweepTargetTimeRemaining <= 0.f)
		{
			CurrentVillageSweepTarget = GetVillageSweepLocation();
			VillageSweepTargetTimeRemaining = VillageSweepTargetDuration;
		}

		FZombieMovementHelperMaasLiam::MoveToLocation(GetOwner(), CurrentVillageSweepTarget, 100.f);

		return;
	}

	FZombieExploreStateMaasLiam::Execute(GetOwner(), TimeSinceLastExploreMove, ExploreMoveInterval, ExploreRadius,
	RecentlyExploredLocations, MaxRecentExploreLocations, ExploreCandidateCount);
}

void UZombieAgentBrainComponentMaasLiam::ExecuteSeekItem()
{
	FZombieSeekItemStateMaasLiam::Execute(GetOwner(), Perceptor, RememberedItems, InventoryComponent);
}

void UZombieAgentBrainComponentMaasLiam::ExecuteFlee()
{
	FZombieFleeStateMaasLiam::Execute(GetOwner(), Perceptor, FleeDistance);
}

void UZombieAgentBrainComponentMaasLiam::ExecuteUseItem()
{
	FZombieSurvivorStatusHelperMaasLiam::TryUseInventoryItem(InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold);
}

FString UZombieAgentBrainComponentMaasLiam::GetStateName() const
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

void UZombieAgentBrainComponentMaasLiam::ExecuteFight(float DeltaTime)
{
	FZombieFightStateMaasLiam::Execute(GetOwner(), Perceptor, InventoryComponent, TimeSinceLastWeaponUse, WeaponUseInterval, FleeDistance);
}

void UZombieAgentBrainComponentMaasLiam::ExecuteSearchHouse()
{
	FZombieSearchHouseStateMaasLiam::Execute(GetOwner(), Perceptor, SearchedHouses, HouseSearchAcceptanceRadius);
}

void UZombieAgentBrainComponentMaasLiam::ExecuteAvoidPurge()
{
	AActor* ClosestPurgeZone = FZombieThreatHelperMaasLiam::GetClosestPurgeZone(Perceptor, GetOwner());
	if (!ClosestPurgeZone) return;

	FZombieMovementHelperMaasLiam::MoveToLocation(GetOwner(), FZombieThreatHelperMaasLiam::GetPurgeAvoidanceLocation(GetOwner(), ClosestPurgeZone, PurgeFleeDistance), 100.f);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Avoiding purge zone"));
}

FVector UZombieAgentBrainComponentMaasLiam::GetVillageSweepLocation() const
{
	const FVector RandomDirection = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal();
	return VillageSweepLocation + RandomDirection * VillageSweepRadius;
}

void UZombieAgentBrainComponentMaasLiam::StartVillageSweep(const FVector& Location)
{
	VillageSweepLocation = Location;
	VillageSweepTimeRemaining = VillageSweepDuration;

	CurrentVillageSweepTarget = GetVillageSweepLocation();
	VillageSweepTargetTimeRemaining = VillageSweepTargetDuration;
}

void UZombieAgentBrainComponentMaasLiam::ExecuteSeekRememberedItem()
{
	FZombieSeekRememberedItemStateMaasLiam::Execute(GetOwner(), Perceptor, RememberedItems, InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold);
}

void UZombieAgentBrainComponentMaasLiam::ExecuteInitialScan(float DeltaTime)
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