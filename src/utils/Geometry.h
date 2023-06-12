//
// Copyright (c) 2008-2020 The Khronos Group Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "Math.hpp"
#include "Params.h"

#include <vector>

namespace Geometry
{
    enum class Shape3D
    {
        Box,
        Sphere
    };

    enum class Plane
    {
        XY,
        YZ,
        XZ
    };

    std::vector<Math::float3> Generate3DGrid(Shape3D shape,
                                             Math::int3 gridRes, Math::float3 gridStartPos, Math::float3 gridEndPos);

    void GenerateBoxGrid(std::vector<Math::float3>& verts, Math::int3 gridRes, Math::float3 gridStartPos, Math::float3 gridEndPos);
    void GenerateSphereGrid(std::vector<Math::float3>& verts, Math::int3 gridRes, Math::float3 gridStartPos, Math::float3 gridEndPos);
}
