#include "ZombieStateSelector.h"

#include "ZombieInventoryHelper.h"
#include "ZombieThreatHelper.h"
#include "ZombieExplorationHelper.h"
#include "ZombieSurvivorStatusHelper.h"

EZombieAgentState FZombieStateSelector::SelectState(
	EZombieAgentState CurrentState,
	AActor* Owner,
	UStudentPerceptor* Perceptor,
	UActorComponent* InventoryComponent,
	UActorComponent* HealthComponent,
	UActorComponent* StaminaComponent,
	const TArray<AActor*>& SearchedHouses,
	float ZombieFightRange,
	float ZombieDangerEnterRange,
	float ZombieDangerExitRange,
	float PurgeDangerRange,
	int LowHealthThreshold,
	float LowStaminaThreshold)
{
	if (!Owner)
	{
		return EZombieAgentState::Explore;
	}

	AActor* ClosestZombie = FZombieThreatHelper::GetClosestZombie(Perceptor, Owner);

	if (ClosestZombie)
	{
		const float ZombieDistance = FVector::Dist(Owner->GetActorLocation(), ClosestZombie->GetActorLocation());

		const bool bHasWeapon = FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Pistol")) || FZombieInventoryHelper::HasInventoryItemType(InventoryComponent, TEXT("Shotgun"));

		const bool bHealthyEnough = FZombieSurvivorStatusHelper::GetCurrentHealth(HealthComponent) > 6;

		const bool bHasEnoughStamina = FZombieSurvivorStatusHelper::GetCurrentStamina(StaminaComponent) > 3.f;

		const bool bCanFight = bHasWeapon && bHealthyEnough && bHasEnoughStamina;

		if (ZombieDistance <= ZombieFightRange && bCanFight)
		{
			return EZombieAgentState::Fight;
		}

		if (ZombieDistance <= ZombieFightRange && !bCanFight)
		{
			return EZombieAgentState::Flee;
		}

		if (CurrentState == EZombieAgentState::Flee)
		{
			if (ZombieDistance <= ZombieDangerExitRange)
			{
				return EZombieAgentState::Flee;
			}
		}
		else if (ZombieDistance <= ZombieDangerEnterRange)
		{
			return EZombieAgentState::Flee;
		}
	}

	if (FZombieSurvivorStatusHelper::ShouldUseItem(InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold))
	{
		return EZombieAgentState::UseItem;
	}

	AActor* ClosestPurgeZone = FZombieThreatHelper::GetClosestPurgeZone(Perceptor, Owner);

	if (ClosestPurgeZone)
	{
		const float PurgeDistance = FVector::Dist(Owner->GetActorLocation(), ClosestPurgeZone->GetActorLocation());

		if (PurgeDistance <= PurgeDangerRange)
		{
			return EZombieAgentState::AvoidPurge;
		}
	}

	AActor* BestItem = FZombieInventoryHelper::GetBestItem(Perceptor, Owner);

	if (BestItem)
	{
		const bool bInventoryFull = FZombieInventoryHelper::IsInventoryFull(InventoryComponent);
		const bool bCanReplace = FZombieInventoryHelper::CanReplaceInventoryItem(InventoryComponent, BestItem);

		if (!bInventoryFull || bCanReplace)
		{
			return EZombieAgentState::SeekItem;
		}
	}

	if (FZombieExplorationHelper::GetClosestUnsearchedHouse(Perceptor, Owner, SearchedHouses))
	{
		return EZombieAgentState::SearchHouse;
	}

	return EZombieAgentState::Explore;
}