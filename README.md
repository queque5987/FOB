# 이슈

## 1. MonsterCharacter의 HP 각 클라이언트에서의 동기화 문제

#### 대미지 전달 시

MonsterCharacter BeginPlay에서 GameState에 Map 형태로 서버에 정보 저장 <this, fHP> <br>
(차후 Monster의 스태미너, 상황 등 struct 선언하여 바꿀 예정) -->

TakeDamage Override하여 Server함수 DamageMonster 호출 -->

GameState에서 HP -= DamageAmount

#### HP 동기화 시 클라이언트에서 동기화를 시키던 문제

MonsterChracter Tick에서 GameState에 접근하여 HP 정보 받아와 멤버변수에 업데이트 -->

각 클라이언트 별 HP 일치X, 나중에 접속한 클라이언트는 HP 100%부터 감소되는 현상 발생 -->

fHP 변수가 Replicated되지 않아서 발생했던 문제였음

### 해결

HP 변수에 UPROPERTY(replicated) 추가 + GetLifetimeReplicatedProps함수 override하여 DOREPLIFETIME(ACMonsterCharacter, fHP) 추가

## 2. Monster의 AnimSequence 각 클라이언트에서 재생 불가 이슈

#### BT_Task Attack 실행 시 애니메이션 Server 동기화 문제

ACMonsterCharacter 클래스에 선언되어 있는 DoAttack Delegate를 Execute하여 AnimSequence를 재생 -->

오프라인에서 정상적으로 애니메이션이 실행되나 Server 상에서 실행되지 않는 현상 발생 -->

Client 상에서만 실행되고 있어 Server -> MultiCast 순으로 실행하여 해결

#### 생성자에서 AnimSequence Load 시 실행 오류

AnimSequence가 저장되어 있는 디렉토리를 AssetRegistryModule을 사용하여 전부 TMap<FString, AnimSequence*> 에 저장하였음 -->

Animation 재생 시 Client 상에서는 실행에 문제 없으나, Server 콘솔에서 Access Violation 발생 -->

Replicated 변수로 만들지 않아 발생한 문제였음

### 해결

TMap은 Replicate를 지원하지 않아 ConstructorHelpers 사용하여 개별적으로 멤버변수에 저장하도록하여 해결

## 3. Monster 공격 판정 이슈

#### Sequence Animation Play 시 Server와 Client 간의 동기화 문제
NotifyState > NotifyTick에서 Bone Location에 충돌 판정을 시도하여 공격 판정을 구현 -->

DrawDebugSphere를 통해 디버그해본 결과 Animation에 따라 Bone Transform이 변하지 않음 -->

SkeletalMesh의 VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones로 변경하여 해결

#### 공격 판정 후 플레이어에게 대미지 전달 시 반응하지 않는 오류
충돌 시 Character 클래스에서 TakeDamage를 직접 호출하여 구현 -->

UGameplayStatics::ApplyPointDamage를 사용하여 내부 함수에서 TakeDamage가 호출되도록 수정 -->

OnTakeAnyDamage Delegate에 PlayerState의 멤버함수를 Bind시켜 PlayerState에 플레이어의 HP를 업데이트하도록 구현하였으나 동작 안 함 -->

PlayerState의 OnTakeAnyDamage에 바인드되어 있었음 -> Character의 OnTakeAnyDamage에 바인드 -->

PlayerState의 BegeinPlay에서 바인드하였으나 BroadCast되었을 때 콜백함수 실행되지 않음 -->

Character가 Possess되기 전에 바인드되어 발생한 문제였음

### 해결

바인드 하는 시기를 BeginPlay보다 늦추기 위해 Character의 PossessedBy에서 바인드하도록 하게 하여 해결

## 4. AimOffset 적용 시 다른 클라이언트에서 반영되지 않는 이슈

#### 초기 구현 방향
AnimInstance에서 내부 변수들을 업데이트, AnimBlueprint에서 Rotator 변수를 사용하여 AimOffset 블렌드를 하는 방향으로 구현 -->

자신의 Animation은 Bind 되나, 다른 클라이언트에서 봤을 때 적용되지 않는 현상 발생

#### 동기화 되지 않는 변수 검출

검색 결과 AnimInstance에서 Replicate를 구현할 수 없다는 점을 알아냄 -->

Character 클래스 Tick에서 ViewRoatation을 주기적으로 업데이트하여 Replicate함 -->

Server에서 캐릭터의 ViewRotation을 구할 수 없으므로, Client에서 ViewRotation을 구한 뒤 Server에서 멤버변수에 할당하는 방식으로 구현 -->

AnimInstance는 Character에서 ViewRotation을 Get하여 AimOffset에 전달

#### 총기 액터 Attach 이후 Socket Transform이 동기화되지 않는 현상

Animation은 ViewRotation에 따라서 동기화되나, 총기 Actor에서 총알 발포 시 ViewRotation이 동기화 되지 않은 Idle 상태의 Socket Transform으로 계산됨 -->

클라이언트에서 서버로 Transform을 전송하여 총알을 Spawn하려 하였으나, 무거워질 것을 염려하여 포기 -->

Animation은 동기화되고 있었으나, SkeletalMesh의 Bone Transoform이 서버에 동기화되지 않고 있음을 확인

### 해결

GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones; 단 한 줄 추가하여 해결

# 기능

## 총기 영점 조정

![aim0ten](https://github.com/user-attachments/assets/e27c2647-439e-4153-82ea-ae7189fa7bc7)

조준점이 향하는 방향과 AimOffset이 (0, 0)일 때 총열이 향하는 방향에 차이를 줄이고자 하였음 -->

CameraComponent로부터 정면으로 LineTrace를 실행하여 충돌하는 액터의 좌표를 감지함(없을 경우 최대 사거리 좌표) -->

위에서 구한 좌표를 GoalZeroPoint라고 했을 때 총열의 방향 벡터와 총열과 GoalZeroPoint 사이의 방향 벡터를 구함 -->

두 방향 벡터 사이의 FRotator를 AimOffset에 적용되는 ViewRotation_Delta에 더하여 해결 -->

가중치에 아크탄젠트를 곱하여 차이가 클수록(영점이 더 엇나갈수록) 변화값이 커지도록 연출

![Zeroing](https://github.com/user-attachments/assets/9d2cfd18-3cdb-4cd9-9e61-b864246bc5b3)

## 2. 총기 전환 UI

![WeaponSwitchUI_0717_move](https://github.com/user-attachments/assets/a92a2467-8cc1-432d-a358-102f9682d499)

FloatingUIActor는 WidgetComponent를 통해 간략한 무기 UI를 띄워주는 액터

PlayerWidgetComponent는 Array에 FloatingUIActor를 저장하고, SplineComponent를 통해 액터를 캐릭터 주변에 회전시킴

#### 회전

FloatingUIActor의 Spline에서의 위치는 UITimes(0.f ~ 1.f)에 의해 결정됨

```C++
1.f / (FloatingUIActorArr.Num() + ((FloatingUIActorArr.Num() % 2 == 0) ? 1.f : 2.f)) * (FloatingUIActorArr.Num() - (i + 2));
```

각 액터는 1 / FloatingUIActor의 개수 (홀수일 경우 +1)의 간격을 가짐

#### 선택

Player의 Scroll 입력을 받아 PlayerWidgetComponent의 SelectedUI(선택된 액터의 인덱스)를 변경함

PlayerWidgetComponent의 Tick에서 현재 Spline 상 중앙에 위치한(0.45f < UITimes < 0.55f) 액터가 SelectedUI와 일치할 경우 회전을 멈추도록 구현
