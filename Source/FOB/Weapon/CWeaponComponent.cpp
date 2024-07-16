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
	if (!GetOwner()->HasAuthority()) return;
	IPlayerCharacter* I_PlayerCharacter = Cast<IPlayerCharacter>(OtherActor);
	if (I_PlayerCharacter == nullptr) return;
	PossessingPlayer = OtherActor;
	I_PlayerCharacter->MulticastRootItem(GetOwner());
}

void UCWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (PossessingPlayer == nullptr)
	//{
	//	GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + FVector(0.f, 0.f, FloatingZ));
	//	FloatingZ += (MaxFloatingZ > 0 ? DeltaTime : -DeltaTime) * FMath::Min(FMath::Atan(-FloatingZ / MaxFloatingZ) + 1.f, 1.f);
	//	if ((MaxFloatingZ < 0 && FloatingZ < MaxFloatingZ) || (MaxFloatingZ > 0 && FloatingZ > MaxFloatingZ))
	//	{
	//		MaxFloatingZ *= -1;
	//	}
	//}
}

void UCWeaponComponent::Fire_Implementation(FVector FireLocation, UStaticMesh* BulletMesh, float Damage)
{
	//UE_LOG(LogTemp, Log, TEXT("UCWeaponComponent - Fire : %s"), *FireLocation.ToString());
	//UE_LOG(LogTemp, Log, TEXT("UCWeaponComponent - Fire On %s : %s"), GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"), *BulletMesh->GetName());

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = PossessingPlayer;
	ACBullet* Bullet = GetWorld()->SpawnActor<ACBullet>(ACBullet::StaticClass(), FireLocation, GetOwner()->GetActorRotation(), SpawnParam);
	if (BulletMesh != nullptr) Bullet->SetStaticMesh(BulletMesh);
}
