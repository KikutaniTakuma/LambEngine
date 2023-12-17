#include <Windows.h>
#include <memory>
#include "World/World.h"
#include "Utils/ExecutionLog/ExecutionLog.h"
#include "Error/Error.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	try {
		auto game = std::make_unique<World>();

		game->Execution();
	}
	catch (const Lamb::Error& err) {
		Lamb::ErrorLog(err);
	}

	return 0;
}