HCMS-2912 디스플레이를 2개 이어 16자리의 숫자를 디스플레이하는 계산기 코드
아두이노 기반. 
키패드 매트릭스를 사용하여 입력을 받고, 매트릭스의 남는 자리는 출력 LED를 밝히는데 사용. 
일반 계산기에서 사칙연산 부호(+-/*) 등을 표시하는 것처럼 LED를 사용해 해당 기호를 표시함. 

PCB 회로 제작 필요. 
임시로 디스플레이 회로만 제작해서 확인해 보는 게 좋을 듯. 

HCMS-2912 데이터 쉬트를 좀 더 살펴봐야 할 것 같다. OSC 와 BLANK 확인할 것. 
Blanks the display when logic high. Can be modulated for brightness control. 라고 나와 있으니 밝기 조절은 BLANK로 하는 것 같다. 
