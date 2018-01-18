// Copyright Project Silver Phoneix

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XBaseCharacter.generated.h"



class ABaseWeapon;
class UCombatComponent;
class UAnimMontage;

USTRUCT(BlueprintType)
struct FCharacterEquipment
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plyaer")
		class ABaseWeapon* CurrentWeapon = nullptr;

};

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	PS_Combat UMETA(DisplayName = "Combat"),
	PS_Passive UMETA(DisplayName = "Passive"),
	PS_Unknown UMETA(DisplayName = "Unknown")
};

UCLASS()
class PROJSILVERPHONEX_API AXBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AXBaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//The weapon that the player starts with
	UPROPERTY(EditDefaultsOnly, Category = "Setup | Weapon")
	TSubclassOf<class ABaseWeapon> StartingWeaponBlueprint;

public:	

	void AddWeaponToInventory(ABaseWeapon* Weapon);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetWalkSpeed(float Speed);

	float GetWalkDirection();

	UFUNCTION(BlueprintCallable, Category = "Character Condition")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Character Condition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Character Condition")
	void SetAddCurrentHealth(float Value);

	UFUNCTION(BlueprintCallable, Category = "Character Condition")
	void SetAddMaxHealth(float Value);




	/*Equipent & Attacking*/

	/** Applies damage to the character */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void AddWeaponToCharacterEquipment(class ABaseWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Animations")
	void AttachWeaponToSocket();

	void SetCanAttack(bool NewState);

	UFUNCTION(BlueprintCallable, Category = "Animations")
	EPlayerStates GetCurrentState() const;

	void SwitchStats(EPlayerStates NewState);

	UPROPERTY(BlueprintReadWrite)
	FCharacterEquipment CharacterEquipment;

	UFUNCTION(BlueprintCallable, Category = "Animations")
	bool GetIsDead() const;

	virtual void OnDeath();

	int32 GetTeamNumber();

	bool CanUnequip() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsFlinching() const;

	bool GetIsInvincible();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerState")
		EPlayerStates CurrentPlayerState;

	UPROPERTY(EditDefaultsOnly, Category = "Setup | Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Setup | Weapon")
	FName BackSocketName;

	UPROPERTY(VisibleAnywhere, Category = "Character Condition")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Character Condition")
	float MaxHealth = 100.f;

	bool bCanAttack = true;

	UPROPERTY(Category = "SetUp", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatStates;

	

private:

	UPROPERTY(Category = "Setup", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		uint8 TeamNumber = 255;

	bool bIsDead = false;


	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		float BackwardsWalkSpeed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true), Category = "Setup | Movement")
		float StrafingSpeed;

	

	


};
