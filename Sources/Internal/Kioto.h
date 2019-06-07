//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

/// Includes all engine public headers.

#pragma once

#include <Core/KiotoEngine.h>
#include <Core/Scene.h>
#include <Core/ECS/SceneSystem.h>

#include <Core/ECS/Entity.h>

#include <Core/ECS/Component.h>
#include <Component/CameraComponent.h>
#include <Component/TransformComponent.h>

#include <Math/MathHelpers.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Matrix4.h>

#include <Render/Renderer.h>
#include <Core/Input/Input.h>

#include <Core/Logger/Logger.h>

using Kioto::byte;
using Kioto::int16;
using Kioto::uint16;
using Kioto::int32;
using Kioto::uint32;
using Kioto::int64;
using Kioto::uint64;

using Kioto::float32;
using Kioto::float64;