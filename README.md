샌드웜이 가장 가까운 유닛을 향해 움직이는 방법:
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
for문을 통해서 맵의 좌표를 탐색
그 좌표에 유닛이 있으면 샌드웜의 현재 위치와 유닛사이의 거리를 계산
계산된 거리가 min_distance보다 작으면 min_distance와 closest_unit을 갱신 -> 샌드웜 이동
