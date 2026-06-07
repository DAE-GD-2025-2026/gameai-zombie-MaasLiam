#include "ZombieStateSelectorMaasLiam.h"

#include "ZombieInventoryHelperMaasLiam.h"
#include "ZombieThreatHelperMaasLiam.h"
#include "ZombieExplorationHelperMaasLiam.h"
#include "ZombieSurvivorStatusHelperMaasLiam.h"

EZombieAgentState FZombieStateSelectorMaasLiam::SelectState(
	EZombieAgentState CurrentState,
	AActor* Owner,
	UStudentPerceptorMaasLiam* Perceptor,
	UActorComponent* InventoryComponent,
	UActorComponent* HealthComponent,
	UActorComponent* StaminaComponent,
	const TArray<AActor*>& SearchedHouses,
	const TArray<FRememberedItem>& RememberedItems,
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
	const float CurrentStamina = FZombieSurvivorStatusHelperMaasLiam::GetCurrentStamina(StaminaComponent);

	const bool bCriticallyLowStamina = CurrentStamina <= 1.f;

	const bool bHasFood = FZombieInventoryHelperMaasLiam::HasInventoryItemType(InventoryComponent, TEXT("Food"));

	if (bCriticallyLowStamina && bHasFood)
	{
		return EZombieAgentState::UseItem;
	}

	AActor* ClosestZombie = FZombieThreatHelperMaasLiam::GetClosestZombie(Perceptor, Owner);

	if (ClosestZombie)
	{
		const float ZombieDistance = FVector::Dist(Owner->GetActorLocation(), ClosestZombie->GetActorLocation());
		const bool bHasUsableWeapon = FZombieInventoryHelperMaasLiam::HasUsableWeapon(InventoryComponent);
		
		const bool bHealthyEnough = FZombieSurvivorStatusHelperMaasLiam::GetCurrentHealth(HealthComponent) > 6;
		const bool bCanFight = bHasUsableWeapon && bHealthyEnough;

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

	if (FZombieSurvivorStatusHelperMaasLiam::ShouldUseItem(InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold))
	{
		return EZombieAgentState::UseItem;
	}

	AActor* ClosestPurgeZone = FZombieThreatHelperMaasLiam::GetClosestPurgeZone(Perceptor, Owner);

	if (ClosestPurgeZone)
	{
		const float PurgeDistance = FVector::Dist(Owner->GetActorLocation(), ClosestPurgeZone->GetActorLocation());

		if (PurgeDistance <= PurgeDangerRange)
		{
			return EZombieAgentState::AvoidPurge;
		}
	}

	AActor* BestItem = FZombieInventoryHelperMaasLiam::GetBestItem(Perceptor, Owner);

	if (BestItem)
	{
		const bool bInventoryFull = FZombieInventoryHelperMaasLiam::IsInventoryFull(InventoryComponent);

		const bool bCanReplace = FZombieInventoryHelperMaasLiam::CanReplaceInventoryItem(InventoryComponent, BestItem);

		if (!bInventoryFull || bCanReplace)
		{
			return EZombieAgentState::SeekItem;
		}
	}
	
	if (FZombieItemMemoryHelperMaasLiam::GetBestRememberedItem(RememberedItems, Owner, InventoryComponent, HealthComponent, StaminaComponent, LowHealthThreshold, LowStaminaThreshold))
	{
		return EZombieAgentState::SeekRememberedItem;
	}

	if (FZombieExplorationHelperMaasLiam::GetClosestUnsearchedHouse(Perceptor, Owner, SearchedHouses))
	{
		return EZombieAgentState::SearchHouse;
	}

	return EZombieAgentState::Explore;
}