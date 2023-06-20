#ifndef EVIE_CORE_H_
#define EVIE_CORE_H_

#ifdef EVIE_PLATFORM_WINDOWS
  #ifdef EVIE_BUILD_DLL
    #define EVIE_API __declspec(dllexport)
  #else
    #define EVIE_API __declspec(dllimport)
  #endif
#else
  #define EVIE_API
#endif

#endif //  EVIE_CORE_H_