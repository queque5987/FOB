#include "Weapon/CXM177_GL.h"
#include "Weapon/CWeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Net/UnrealNetwork.h"


ACXM177_GL::ACXM177_GL()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	// Component Setting
	WeaponComponent = CreateDefaultSubobject<UCWeaponComponent>(TEXT("WeaponComponent"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent_Magazine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Magazine"));
	StaticMeshComponent_GranadeLauncher = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_GranadeLauncher"));
	//StaticMeshComponent->SetIsReplicated(true);

	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent_Magazine->SetupAttachment(RootComponent);
	WeaponComponent->SetupAttachment(RootComponent);
	StaticMeshComponent_GranadeLauncher->SetupAttachment(RootComponent);

	// StaticMesh Setting
	ConstructorHelpers::FObjectFinder<UStaticMesh> SMFinder(TEXT("/Game/Resources/Weapons/xm177/xm177_with_gl_fbx_2018"));
	if (SMFinder.Succeeded()) StaticMeshComponent->SetStaticMesh(SMFinder.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SM_MagFinder(TEXT("/Game/Resources/Weapons/xm177/20roundmagazine_fbx_2018"));
	if (SM_MagFinder.Succeeded()) StaticMeshComponent_Magazine->SetStaticMesh(SM_MagFinder.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BulletFinder(TEXT("/Game/Resources/Weapons/xm177/556_cartridge_fbx_2018_Projectile"));
	if (SM_BulletFinder.Succeeded()) BulletStaticMesh = SM_BulletFinder.Object;

	ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GLFinder(TEXT("/Game/Resources/Weapons/xm177/xm148_fbx_2018"));
	if (SM_GLFinder.Succeeded()) StaticMeshComponent_GranadeLauncher->SetStaticMesh(SM_GLFinder.Object);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GranadeFinder(TEXT("/Game/Resources/Weapons/xm177/40mmgrenade_fbx_2018"));
	if (SM_GranadeFinder.Succeeded()) GranadeStaticMesh = SM_GranadeFinder.Object;
}

void ACXM177_GL::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACXM177_GL::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FVector ACXM177_GL::GetFireSocketPos()
{
	return StaticMeshComponent->GetSocketLocation("FireSocket");
}

void ACXM177_GL::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACXM177_GL::LMBTriggered_Implementation()
{
	if (WeaponComponent == nullptr) return;

	FVector FirePos = StaticMeshComponent->GetSocketLocation("FireSocket");

	//UE_LOG(LogTemp, Log, TEXT("%s\tACXM177 Firesocket- LMBTriggered At %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"), *FirePos.ToString());
	//UE_LOG(LogTemp, Log, TEXT("%s\tACXM177 StaticMesh- LMBTriggered At %s"), HasAuthority() ? TEXT("Server") : TEXT("Client"), *GetActorLocation().ToString());

	WeaponComponent->Fire(
		FirePos,
		BulletStaticMesh,
		1.f
	);
}

