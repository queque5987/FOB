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
