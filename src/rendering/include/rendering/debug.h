#ifndef EVIE_INCLUDE_RENDERING_DEBUG_H_
#define EVIE_INCLUDE_RENDERING_DEBUG_H_

#include <source_location>
#include <string>

#include "evie/error.h"
#include "evie/logging.h"

#include "glad/glad.h"

namespace evie {

Error EVIE_API glCheckError();

template<typename func, typename... Ts> struct EVIE_API CallOpenGL
{
  CallOpenGL(func&& f, Ts&&... ts, const std::source_location& loc = std::source_location::current())
  // CallOpenGL(func&& f, Ts&&... ts)
  {
    f(ts...);
    Error error = glCheckError();
    if (error.Bad()) {
      // EV_ERROR("OPENGL ERROR - {}", error.Message());
      EV_ERROR("OPENGL ERROR - File: {}({}:{}) `{}`: {}",
        loc.file_name(),
        loc.line(),
        loc.column(),
        loc.function_name(),
        error.Message());
        //assert(false);
    }
  }
};

template<typename func, typename... Ts> CallOpenGL(func&& f, Ts&&... args) -> CallOpenGL<func, Ts...>;

}// namespace evie

#endif// !EVIE_INCLUDE_RENDERING_DEBUG_H_