#ifndef EVIE_CORE_H_
#define EVIE_CORE_H_


// Check gcc.gnu.org/wiki/Visibility for more details

#ifdef EVIE_PLATFORM_WINDOWS
  #ifdef EVIE_BUILD_DLL
    #define EVIE_API __declspec(dllexport)
  #else
    #define EVIE_API __declspec(dllimport)
  #endif
#else
  #if __GNUC__ >= 4
    #define EVIE_API __attribute__((visibility("default")))
  #else
    #define EVIE_API
  #endif
#endif

#endif //  EVIE_CORE_H_