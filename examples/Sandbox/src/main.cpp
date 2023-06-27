#include <memory>

#include "evie/evie.h"
#include "evie/logging.h"

class Sandbox : public evie::Application
{
public:
private:
};

std::unique_ptr<evie::Application> CreateApplication() { return std::make_unique<Sandbox>(); }
