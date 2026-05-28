#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZombieAgentBrainComponent.generated.h"

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

	void UpdateState();
	FString GetStateName() const;
};