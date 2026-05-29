#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "StudentPerceptor.generated.h"

USTRUCT(BlueprintType)
struct FZombieThreatMemory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	AActor* Zombie = nullptr;

	UPROPERTY(BlueprintReadOnly)
	float TimeSinceLastSeen = 0.f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAASLIAMZOMBIERUNTIME_API UStudentPerceptor : public UActorComponent
{
	GENERATED_BODY()

public:
	UStudentPerceptor();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SeenZombies;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SeenItems;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SeenHouses;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SeenPurgeZones;

private:
	UPROPERTY()
	TArray<FZombieThreatMemory> ZombieMemories;

	float ZombieMemoryDuration = 8.f;

	bool IsZombieActor(AActor* Actor) const;
	void RememberZombie(AActor* Zombie);
	void UpdateZombieMemory(float DeltaTime);
};