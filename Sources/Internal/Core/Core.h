//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#ifdef KIOTOENGINE_EXPORTS
#define KIOTO_API __declspec(dllexport)
#else
#define KIOTO_API __declspec(dllimport)
#endif