#pragma once

#include <string>
#include <vector>

#include "Core/Core.h"

namespace Kioto
{

std::vector<std::string> m_types;

#define TYPE_TO_NAME(type, ret) ret = #type;

#define REGISTER_TYPE(type) \
}
}