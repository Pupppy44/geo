#include "studio/studio.h"

#ifdef _DEBUG
int main() {
#else
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
#endif
	studio::studio studio;
	studio.init();
	studio.start();

	return 0;
}