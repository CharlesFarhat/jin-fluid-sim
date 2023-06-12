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

#include "Geometry.h"
#include "Logger.h"
#include <cmath>

namespace Geometry
{

    std::vector<Math::float3> Generate3DGrid(Shape3D shape,
                                             Math::int3 gridRes, Math::float3 gridStartPos, Math::float3 gridEndPos)
    {
        if (gridRes.x * gridRes.y * gridRes.z <= 0)
        {
            LOG_ERROR("Cannot generate grid with negative or null number of vertices");
            std::vector<Math::float3> nullVec;
            return nullVec;
        }

        std::vector<Math::float3> verts(gridRes.x * gridRes.y * gridRes.z, Math::float3(0.0f, 0.0f, 0.0f));

        switch (shape)
        {
            case Shape3D::Sphere:
                GenerateSphereGrid(verts, gridRes, gridStartPos, gridEndPos);
                break;
            case Shape3D::Box:
                GenerateBoxGrid(verts, gridRes, gridStartPos, gridEndPos);
            default:
                break;
        }

        return verts;
    }

    void GenerateBoxGrid(std::vector<Math::float3>& verts, Math::int3 gridRes, Math::float3 gridStartPos, Math::float3 gridEndPos)
    {
        if (verts.size() < gridRes.x * gridRes.y * gridRes.z)
        {
            LOG_ERROR("Cannot generate grid with this resolution");
            return;
        }

        Math::float3 vec = gridEndPos - gridStartPos;
        Math::float3 gridSpacing = Math::float3({ vec.x / gridRes.x, vec.y / gridRes.y, vec.z / gridRes.z });

        int vertIndex = 0;
        for (int ix = 0; ix < gridRes.x; ++ix)
        {
            for (int iy = 0; iy < gridRes.y; ++iy)
            {
                for (int iz = 0; iz < gridRes.z; ++iz)
                {
                    verts[vertIndex++] = {
                            gridStartPos.x + ix * gridSpacing.x,
                            gridStartPos.y + iy * gridSpacing.y,
                            gridStartPos.z + iz * gridSpacing.z
                    };
                }
            }
        }
    }

    void GenerateSphereGrid(std::vector<Math::float3>& verts, Math::int3 gridRes, Math::float3 gridStartPos, Math::float3 gridEndPos)
    {
        if (verts.size() < gridRes.x * gridRes.y * gridRes.z)
        {
            LOG_ERROR("Cannot generate grid with this resolution");
            return;
        }

        Math::float3 vec = gridEndPos - gridStartPos;
        Math::float3 gridCenterPos = gridStartPos + vec / 2.0f;
        float radius = Math::length(vec) / 2.0f;
        float phiSpacing = Math::PI_F / gridRes.x;
        float thetaSpacing = 2.0f * Math::PI_F / gridRes.y;
        float radiusSpacing = radius / gridRes.z;

        int vertIndex = 0;
        for (int iphi = 0; iphi < gridRes.x; ++iphi)
        {
            for (int itheta = 0; itheta < gridRes.y; ++itheta)
            {
                for (int ir = 0; ir < gridRes.z; ++ir)
                {
                    verts[vertIndex++] = {
                            gridCenterPos.x + ((ir + 1) * radiusSpacing) * std::cos(itheta * thetaSpacing) * std::sin(iphi * phiSpacing),
                            gridCenterPos.y + ((ir + 1) * radiusSpacing) * std::sin(itheta * thetaSpacing) * std::sin(iphi * phiSpacing),
                            gridCenterPos.z + ((ir + 1) * radiusSpacing) * std::cos(iphi * phiSpacing),
                    };
                }
            }
        }
    }
}