//C언어 제공 타이머 사용하기
#include <stdio.h>
#include <windows.h>
#include <time.h>

int main() {
	int countTime = 5;
	while (1) {
		printf("%d초\n", countTime);
		Sleep(1000);
		countTime -= 1;

		if (countTime <= 0)
		{
			printf("end!\n");
			return 0;
		}
	}
}
