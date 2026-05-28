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

	float TimeSinceLastExploreMove = 0.f;
	float ExploreMoveInterval = 3.f;
	float ExploreRadius = 1200.f;

	void UpdateState();
	void ExecuteCurrentState(float DeltaTime);

	void ExecuteExplore(float DeltaTime);
	void ExecuteSeekItem();

	AActor* GetClosestItem() const;
	UActorComponent* FindInventoryComponent() const;

	bool TryPickupItem(AActor* ItemActor);
	bool TryGrabItemInSlot(int32 SlotIndex, AActor* ItemActor);
	float GetPickupRange() const;
	int32 GetInventoryCapacity() const;

	FVector GetRandomExploreLocation() const;
	FString GetStateName() const;
};