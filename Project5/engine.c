//건물, 지형 배치 129 ~ 140
//단축키 입력 처리 60 ~ 77
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };
POSITION selected_position = { -1, -1 };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300
};

SANDWORM sandworm = { {10, 10}, 1, 1, 500 };

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case ' ':
				if (map[0][cursor.current.row][cursor.current.column] != ' ') {
					selected_position = cursor.current;
					printf("지형 또는 오브젝트 선택됨: (%d, %d)\n", selected_position.row, selected_position.column);
					display_status_window(selected_position);
				}
				break;
			case 27: 
				selected_position.row = -1;
				selected_position.column = -1;
				display_status_window(selected_position);
				printf("선택 취소됨\n");
				break;
			case 'H':
				printf("Harvester를 생산합니다.\n");
				break;
			case 'P':
				printf("Plate를 건설합니다.\n");
				break;
			case 'F':
				printf("Fremen을 생산합니다.\n");
				break;
			case 'S':
				printf("Soldier를 생산합니다.\n");
				break;
			case 'T':
				printf("Heavy Tank를 생산합니다.\n");
				break;
			case 'G':
				printf("Fighter를 생산합니다.\n");
				break;
			case k_quit: outro(); break;
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		sandworm_move();

		// 화면 출력
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf("DUNE 1.5\n");
	Sleep(2000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])에 지형 생성
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])은 비워 두기(-1로 채움)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	
	map[0][1][1] = 'B';
	map[0][2][2] = 'H';
	map[0][3][3] = '5';
	map[0][1][3] = 'P';
	map[0][2][3] = 'P';
	map[0][1][4] = 'P';
	map[0][2][4] = 'P';
	map[0][5][5] = 'R';
	map[0][7][7] = 'R';
	map[0][9][9] = 'R';
	map[0][4][4] = 'W';
	map[0][6][6] = 'W';

	// object sample
	map[1][obj.pos.row][obj.pos.column] = 'o';
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	POSITION new_pos = pmove(curr, dir);

	// validation check
	if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
		1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

		cursor.previous = cursor.current;
		cursor.current = new_pos;
	}
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}
//샌드웜 이동 - 가장 가까운 유닛 탐색 -> 해당 방향으로 이동, 잡아먹으면 커지고 가끔 매장지 생성
void sandworm_move(void) {
	if (sys_clock % sandworm.speed != 0) {
		return; 
	}
	POSITION closest_unit = { -1, -1 };
	int min_distance = MAP_HEIGHT * MAP_WIDTH;

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (map[1][i][j] == 'H' || map[1][i][j] == 'F' || map[1][i][j] == 'S' || map[1][i][j] == 'T' || map[1][i][j] == 'G') {
				int distance = abs(sandworm.pos.row - i) + abs(sandworm.pos.column - j);
				if (distance < min_distance) {
					min_distance = distance;
					closest_unit.row = i;
					closest_unit.column = j;
				}
			}
		}
	}
	if (closest_unit.row != -1 && closest_unit.column != -1) {
		if (sandworm.pos.row < closest_unit.row) {
			sandworm.pos.row++;
		}
		else if (sandworm.pos.row > closest_unit.row) {
			sandworm.pos.row--;
		}

		if (sandworm.pos.column < closest_unit.column) {
			sandworm.pos.column++;
		}
		else if (sandworm.pos.column > closest_unit.column) {
			sandworm.pos.column--;
		}

		if (sandworm.pos.row == closest_unit.row && sandworm.pos.column == closest_unit.column) {
			map[1][closest_unit.row][closest_unit.column] = -1;
			sandworm.length += sandworm.growth_rate;

			if (rand() % 5 == 0) {
				map[0][closest_unit.row][closest_unit.column] = '5';
				printf("스파이스 매장지가 생성되었습니다.\n");
			}
		}
	}
}