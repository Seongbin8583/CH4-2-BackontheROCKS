#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BOTRPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;
class UParticleSystem;
class USoundBase;
class ABOTRPlayerState;

UCLASS(Abstract)
class BACKONTHEROCKS_API ABOTRPickup : public AActor
{
    GENERATED_BODY()

public:
    ABOTRPickup();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<USphereComponent> Trigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<UStaticMeshComponent> ItemMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup|FX")
    TObjectPtr<UParticleSystem> PickupParticle;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup|FX")
    TObjectPtr<USoundBase> PickupSound;

    virtual bool GiveReward(ABOTRPlayerState* PS);

    UFUNCTION()
    void OnPickupOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION(NetMulticast, Reliable)
    void MulticastCollected();

private:
    bool bCollected = false;
};