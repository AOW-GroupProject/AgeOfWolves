## GitHub 정책, [24-04-06, Updated by YongSeol]

관련 링크 : https://webddevys.tistory.com/440

### 브랜치 정책

1. main 

	- 개념 : 배포 용 최종 코드가 관리되는 브랜치, 절대 건드리지 않는다.
	- 보호 정책 : main은 Lock, 절대 최종 코드가 아닌 이상 push하지 않는다.

2. develop 

	- 개념 : 실질적으로 개발이 현재 진행되는 브랜치, 거의 완성된 코드만 올라간다.
	- 보호 정책 : develp도 Lock, PR 필수, 코드 리뷰 받기, Approval 모두 받기 그 다음에 Merge

3. feature

	- 개념 : 이슈로부터 생성되어, develop 브랜치를 Base로 두는 각 개발자의 Local에서 작업을 수행하는 Branch입니다. 코드 리뷰 후 develop 브랜치로의 PR이 승인되면, 성공적으로 develop에 merge되어 즉시 관련 이슈와 함께 제거됩니다.
	- 이름 형식 : <이슈종류>-<마일스톤><이슈번호> 형식, 예제) feature-CharacterLocomotion#5, fix-CharacterLocomotion#5
	- base 브랜치/상위 브랜치 : develop 브랜치
	- 보호 정책 : 각 개발자가 소유하는 Local 브랜치 개념으로, 서로의 브랜치는 최대한 침범하지 않는다. 
	- 매커니즘
		
  		1. Issue 발행 // Owner가 발행, 회의를 통해 결정, 신경 안써도됨
		2. GitHub Project에서 해당 이슈를 On-Progress 칼럼으로 옮긴다.
		3. Assignee 설정 - 작업하는 본인으로 변경
		3. Create Branch 
		4. Base 브랜치를 Develop으로 설정 
		5. Local에서 작업
		6. 작업 완료 후 Commit 작성 및 Push (원격 저장소의 동일 네임의 feature 브랜치로 push)
		7. develop < - feature PR 작성 후, 리뷰 받기
		8. 모든 리뷰어로부터 Approval 받으면, Owner가 Merge 진행

************************************************************************************************************************************************************************

2. Commit Convention

	1. Commit 제목
		
		a) Feat : 기능 추가, 구현
		b) Fix : 버그 수정 
		c) Test : 테스트 코드
		d) Asset : 애셋 추가/수정/변경 (비 개발자 전용)
		e) Sync : Conflict 발생 시 브랜치 간 동기화 작업 (Owner 전용)

	2. Commit 본문 : 이전 Commit 참조

************************************************************************************************************************************************************************

3. PR 작성

	1. 제목 작성 유형

		- [Commit 제목], 관련 이슈: <이슈번호>

		예제) Feat : Character Locomotion 구현, 관련 이슈:#5

	2. 본문

		- PR 템플릿 활용

************************************************************************************************************************************************************************



