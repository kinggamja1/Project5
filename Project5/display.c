/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };

const POSITION status_window_pos = { MAP_HEIGHT + 2, 0 };
const POSITION system_message_pos = { MAP_HEIGHT + 8, 0 };
const POSITION command_window_pos = { MAP_HEIGHT + 10, 0 };


char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_status_window(void);
void display_system_message(void);
void display_commands(void);


void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_status_window();
	display_system_message();
	display_commands();
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				char ch = backbuf[i][j];
				printc(padd(map_pos, pos), backbuf[i][j], COLOR_DEFAULT);

				switch (ch) {
				case 'B': 
					set_color(COLOR_BLUE); 
					break;
				case 'H':
					set_color(COLOR_RED);   
					break;
				case '5':
					set_color(COLOR_YELLOW); 
					break;
				case 'P': 
					set_color(COLOR_BLACK); 
					break;
				case 'R': 
					set_color(COLOR_GRAY);   
					break;
				case 'W': 
					set_color(COLOR_ORANGE); 
					break;
				default:
					set_color(COLOR_DEFAULT);
					break;
				}

				printc(padd(map_pos, pos), ch, -1);
			}
			frontbuf[i][j] = backbuf[i][j];
			}
			
		}
	}


// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}
void display_status_window(void) {
	set_color(COLOR_DEFAULT);
	gotoxy(status_window_pos);
	printf("상태창: 선택된 유닛/지형 정보 및 상태");
}

void display_system_message(void) {
	set_color(COLOR_DEFAULT);
	gotoxy(system_message_pos);
	printf("시스템 메시지: 여기에 시스템 메시지를 출력");
}

void display_commands(void) {
	set_color(COLOR_DEFAULT);
	gotoxy(command_window_pos);
	printf("명령창: 내릴 수 있는 명령을 표시");
}