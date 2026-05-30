#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZombieAgentBrainComponent.generated.h"

class UStudentPerceptor;

UENUM(BlueprintType)
enum class EZombieAgentState : uint8
{
	Explore UMETA(DisplayName = "Explore"),
	SeekItem UMETA(DisplayName = "Seek Item"),
	Flee UMETA(DisplayName = "Flee"),
	Fight UMETA(DisplayName = "Fight"),
	UseItem UMETA(DisplayName = "Use Item"),
	SearchHouse UMETA(DisplayName = "Search House"),
	AvoidPurge UMETA(DisplayName = "Avoid Purge")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAASLIAMZOMBIERUNTIME_API UZombieAgentBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UZombieAgentBrainComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void StartVillageSweep(const FVector& Location);

private:
	EZombieAgentState CurrentState = EZombieAgentState::Explore;

	UPROPERTY()
	UStudentPerceptor* Perceptor = nullptr;

	UPROPERTY()
	UActorComponent* InventoryComponent = nullptr;

	UPROPERTY()
	UActorComponent* HealthComponent = nullptr;

	UPROPERTY()
	UActorComponent* StaminaComponent = nullptr;

	UPROPERTY()
	TArray<AActor*> SearchedHouses;

	float TimeSinceLastExploreMove = 0.f;
	float ExploreMoveInterval = 3.f;
	float ExploreRadius = 2200.f;

	float ZombieDangerEnterRange = 850.f;
	float ZombieDangerExitRange = 1400.f;
	float FleeDistance = 1600.f;

	float ZombieFightRange = 450.f;
	float TimeSinceLastWeaponUse = 0.f;
	float WeaponUseInterval = 0.75f;

	int LowHealthThreshold = 6;
	float LowStaminaThreshold = 4.f;

	float HouseSearchAcceptanceRadius = 150.f;

	float PurgeDangerRange = 900.f;
	float PurgeFleeDistance = 1400.f;
	
	float MinimumSprintStamina = 2.f;
	
	FVector VillageSweepLocation = FVector::ZeroVector;
	FVector CurrentVillageSweepTarget = FVector::ZeroVector;

	float VillageSweepTimeRemaining = 0.f;
	float VillageSweepTargetTimeRemaining = 0.f;

	float VillageSweepDuration = 10.f;
	float VillageSweepRadius = 800.f;
	float VillageSweepTargetDuration = 2.5f;

	void UpdateState();
	void ExecuteCurrentState(float DeltaTime);

	void ExecuteExplore(float DeltaTime);
	void ExecuteSeekItem();
	void ExecuteFlee();
	void ExecuteFight(float DeltaTime);
	void ExecuteUseItem();
	void ExecuteSearchHouse();
	void ExecuteAvoidPurge();
	FString GetStateName() const;
	FVector GetVillageSweepLocation() const;
};