#pragma once

/*
	AquaShell (dnyAquaShell) developed by Daniel Brendel

	(C) 2017 - 2025 by Daniel Brendel

	Version: 1.0
	Contact: dbrendel1988<at>gmail<dot>com
	GitHub: https://github.com/danielbrendel/

	Released under the MIT license
*/

#define DNY_AS_PRODUCT_NAME L"AquaShell"
#define DNY_AS_PRODUCT_CODENAME L"dnyAquaShell"
#define DNY_AS_PRODUCT_VERSION L"1.0"
#define DNY_AS_PRODUCT_AUTHOR L"Daniel Brendel"
#define DNY_AS_PRODUCT_CONTACT L"https://www.danielbrendel.com"
#define DNY_AS_PRODUCT_REPOSITORY L"https://github.com/danielbrendel/dnyAquaShell"
#define DNY_AS_PRODUCT_LICENCE L"MIT"

#define DNY_AS_PRODUCT_VERSION_W MAKEWORD(1, 0)

#if defined(_WIN64)
	#define PLATFORM_NAME "x64"
#elif defined(_WIN32)
	#define PLATFORM_NAME "x86"
#endif

#if defined(_DEBUG)
	#define BUILD_TYPE "Debug"
#elif defined(NDEBUG)
	#define BUILD_TYPE "Release"
#endif
