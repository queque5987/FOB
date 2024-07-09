## 1. MonsterCharacter의 HP 각 클라이언트에서의 동기화

#### 대미지 전달 시

MonsterCharacter BeginPlay에서 GameState에 Map 형태로 서버에 정보 저장 <this, fHP> <br>
(차후 Monster의 스태미너, 상황 등 struct 선언하여 바꿀 예정) -->

TakeDamage Override하여 Server함수 DamageMonster 호출 -->

GameState에서 HP -= DamageAmount

#### HP 동기화 시

MonsterChracter Tick에서 GameState에 접근하여 HP 정보 받아와 멤버변수에 업데이트 -->

각 클라이언트 별 HP 일치X, 나중에 접속한 클라이언트는 HP 100%부터 감소되는 현상 발생 -->

fHP 변수가 Replicated되지 않아서 발생했던 문제였음

### 해결

HP 변수에 UPROPERTY(replicated) 추가 + GetLifetimeReplicatedProps함수 override하여 DOREPLIFETIME(ACMonsterCharacter, fHP) 추가

## 2. Monster의 AnimSequence 각 클라이언트에서 재생

#### BT_Task Attack 실행 시
ACMonsterCharacter 클래스에 선언되어 있는 DoAttack Delegate를 Execute하여 AnimSequence를 재생 -->

오프라인에서 정상적으로 애니메이션이 실행되나 Server 상에서 실행되지 않는 현상 발생 -->

Client 상에서만 실행되고 있어 Server -> MultiCast 순으로 실행하여 해결

#### 생성자에서 AnimSequence Load 시

AnimSequence가 저장되어 있는 디렉토리를 AssetRegistryModule을 사용하여 전부 TMap<FString, AnimSequence*> 에 저장하였음 -->

Animation 재생 시 Client 상에서는 실행에 문제 없으나, Server 콘솔에서 Access Violation 발생 -->

Replicated 변수로 만들고자 하였으나 TMap은 지원하지 않음 -->

ConstructorHelpers 사용하여 개별적으로 멤버변수에 저장하도록하여 해결
