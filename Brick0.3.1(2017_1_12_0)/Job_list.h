/*

벽돌 속성을 추가할 것.
------
공의 속도가 일정시간 줄지 않는다.
폭탄 벽돌.(직선, 사각형)
대형 벽돌(옆 벽돌과 하나로 따지는 코드가 있어야 하기 때문에, Play_brick에도 코드 변경 필요.).
공의 속도가 증가한다.
반사판이 계속 돈다
반사판이 반대로 움직인다
반사판이 크기가 변경된다
반사판이 이동속도가 달라진다.
------

#########

파일 암호화를 약간만 더 복잡하게 하기(hex확인시 너무 규칙적으로 보임).

반사판이 공을 밀 떄의 공의 움직임을 좀 더 자연스럽게 할 것.(물리적으로).

공이 수직-수평 진행 중일 때, Reflector의 밀림에 의해 Hitting_zone으로 영영 들어오지 못하게 되는 버그를 고칠 것.(일단 고쳤으나, 더 많은 테스트 필요.)

출력 속도가 느린 점을 고칠 것.(Need Performance Upgrade)(일단 1차 업그레이드 완료.)(2차 업그레이드 완료.(화면 출력 스트림 더 최적화.))
--->3차 업그레이드로 출력을 할 수 있는 시간 동안만 출력함으로 함으로써, 시간지연은 막음.--->어지간하면 정상적으로 보임.
-->직접 시간 측정 결과 Show_opt_kr이 시간의 70~90% 차지함(시간제한없이 출력 시).

#########

공이 빨간색일 경우 출력오류 발견.-->공 최대 속도 부분의 문제로 추정, 수정완료. 이후 사라졌으나 지속적 확인 필요.

사운드 기능.

*/