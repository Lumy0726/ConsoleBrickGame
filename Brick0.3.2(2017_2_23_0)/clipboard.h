/*
�� �� �� �� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� �� �� �� ��
�� �� �� �� ��
�� �� �� �� ��
�� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� ��
�� �� �� �� �� �� �� �� �� �� �� ��
��ܡ���������ۡ�䢷����墸����
*/
#include <vector>
std::vector<int> s;
typedef std::vector<int> asdfasdf;
//ó���� ���� 312����.

typedef enum{
BLACK,
D_BLUE,//1
D_GREEN,
D_SKYBLUE,
D_RED,
D_VIOLET,//5
D_YELLOW,
GREY,
D_GREY,
BLUE,
GREEN,//10
SKYBLUE,
RED,
VIOLET,
YELLOW,
WHITE,//15
	} T_color;

enum{
	BOARD_VERGE = 18,//ȭ�� ���� ���� ��.
	OUTPUT_TICK = 30,//ȭ�� ��°���.(���� ms).
	BRICK_SIZE = 5,//"�ϳ��� ������ ��� �����ΰ�?".
	//5�� ��������� ����� ��(�������). �׷��� ������ �۰� �������� ����.
	//6�� ¦������ �������� �ְ�� ���ɼ��� ����.
	//7�� Ȧ���̸鼭 Ŀ�� �������� ������ ��������� ���� ��.
	INTERNAL_RESOLUTION = 100,//¦�� �̾����.
	//�ʼ�����. BRICK_SIZE*INTERNAL_RESOLUTON >= VECTOR_LENGTH.
	BALL_MAX_NUM = 6,//�� �ִ� ����.
	LEVELUP_SEC = 20,//������ �ֱ�.
	PLAYBRICK_SPEED_TICK_START = 300,
	PLAYBRICK_SPEED_TICK_END = 160,
};

enum{
	SHADOW_NUM = 50,//�׸��ڼ�.
	DIRECTION_NUM = 40,//���� ���� ��.
};

template <typename T>
void C_queue_dump<T>::Dump(char * Ptr) const{
	unsigned Loop1;
	*(unsigned *)Ptr = Dump_size();
	Ptr += sizeof(unsigned);
	for (Loop1 = 0; Loop1 < Num(); Loop1++){
		Read(Loop1).Dump(Ptr);
		Ptr += Read(Loop1).Dump_size();
	}
}
template <typename T>
bool C_queue_dump<T>::Restore(char * Ptr){
	char * Ptr_E1, *Ptr_E2;
	unsigned Loop1;
	Ptr_E1 = Ptr + *(unsigned *)Ptr;
	Ptr_E2 = Ptr + sizeof(unsigned);
	while (Num() != 0){ Out(); }
	for (Loop1 = 0; true; Loop1++){
		In(T());
		if (!(Read_editable(Loop1)->Restore(Ptr_E2))){
			return false;
		}
		Ptr_E2 = Ptr_E2 + *(unsigned *)Ptr_E2;
		if (Ptr_E1 == Ptr_E2){
			break;
		}
		else if (Ptr_E1 < Ptr_E2){
			return false;
		}
		if (Num() == Size - 1){ return false; }
	}
	return true;
}