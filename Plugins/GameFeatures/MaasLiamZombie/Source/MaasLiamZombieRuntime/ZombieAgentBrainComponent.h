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
	UseItem UMETA(DisplayName = "Use Item")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAASLIAMZOMBIERUNTIME_API UZombieAgentBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UZombieAgentBrainComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

	float TimeSinceLastExploreMove = 0.f;
	float ExploreMoveInterval = 3.f;
	float ExploreRadius = 1200.f;

	float ZombieDangerEnterRange = 700.f;
	float ZombieDangerExitRange = 1200.f;
	float FleeDistance = 1600.f;

	int LowHealthThreshold = 5;
	float LowStaminaThreshold = 4.f;

	void UpdateState();
	void ExecuteCurrentState(float DeltaTime);

	void ExecuteExplore(float DeltaTime);
	void ExecuteSeekItem();
	void ExecuteFlee();
	void ExecuteUseItem();

	AActor* GetBestItem() const;
	int32 GetItemPriority(AActor* ItemActor) const;
	AActor* GetClosestZombie() const;

	UActorComponent* FindComponentByNamePart(const FString& NamePart) const;

	bool TryPickupItem(AActor* ItemActor);
	bool TryGrabItemInSlot(int32 SlotIndex, AActor* ItemActor);

	bool ShouldUseItem() const;
	bool TryUseInventoryItem();
	bool TryUseItemInSlot(int32 SlotIndex);
	bool TryRemoveItemInSlot(int32 SlotIndex);

	float GetPickupRange() const;
	int32 GetInventoryCapacity() const;

	int GetCurrentHealth() const;
	float GetCurrentStamina() const;

	FVector GetRandomExploreLocation() const;
	FVector GetFleeLocation(AActor* ZombieActor) const;
	float ScoreFleeLocation(const FVector& Location) const;

	FString GetStateName() const;
};