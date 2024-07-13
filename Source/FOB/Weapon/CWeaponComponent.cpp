#include "Weapon/CWeaponComponent.h"
#include "Components/SphereComponent.h"
#include "Player/PlayerCharacter.h"
#include "CBullet.h"

UCWeaponComponent::UCWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetSphereRadius(200.f);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	Collider->SetupAttachment(this);
}

void UCWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &UCWeaponComponent::OnComponentBeginOverlap);
}

void UCWeaponComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPlayerCharacter* I_PlayerCharacter = Cast<IPlayerCharacter>(OtherActor);
	if (I_PlayerCharacter == nullptr) return;
	PossessingPlayer = OtherActor;
	I_PlayerCharacter->ServerEquipItem(GetOwner());
}

void UCWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCWeaponComponent::Fire_Implementation(FVector FireLocation, UStaticMesh* BulletMesh, float Damage)
{
	//UE_LOG(LogTemp, Log, TEXT("UCWeaponComponent - Fire : %s"), *FireLocation.ToString());
	//UE_LOG(LogTemp, Log, TEXT("UCWeaponComponent - Fire On %s : %s"), GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"), *BulletMesh->GetName());

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = PossessingPlayer;
	ACBullet* Bullet = GetWorld()->SpawnActor<ACBullet>(ACBullet::StaticClass(), FireLocation, GetOwner()->GetActorRotation(), SpawnParam);
	Bullet->SetStaticMesh(BulletMesh);
	//Bullet->SetReplicates(true);
}
