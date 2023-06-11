/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/
#include "Camera.h"

using namespace Render;
using namespace Math;

Camera::Camera(float sceneAspectRatio)
        : m_fov(120.0f)
        , m_aspectRatio(sceneAspectRatio)
        , m_zNear(0.01f)
        , m_zFar(6000.f)
        , m_cameraInitPos({ 25.0, -1.0, 0.0 })
        , m_focusInitPos({ 0.0, 0.0, 0.0 })
        , m_isAutoRotating(false)
{
    reset();
}

void Camera::reset()
{
    m_cameraPos = m_cameraInitPos;
    m_focusPos = m_focusInitPos;
    updateProjMat();
    updateProjViewMat();
}

void Camera::rotate(float angleX, float angleY)
{
    float3 vecTargetCamera(m_cameraPos - m_focusPos);

    auto rot = float4x4::RotationY(angleY) * float4x4::RotationX(angleX);

    const auto matWorldToCam = m_viewMat.RemoveTranslation();
    rot = matWorldToCam * rot * matWorldToCam.Transpose();

    vecTargetCamera = vecTargetCamera * rot;
    m_cameraPos = m_focusPos + vecTargetCamera;

    updateProjViewMat();
}

void Camera::translate(float dispX, float dispY)
{
    auto trans = float4x4::Translation(dispX, dispY, 0.0f);

    const auto matWorldToCam = m_viewMat.RemoveTranslation();
    trans = matWorldToCam * trans * matWorldToCam.Transpose();

    m_cameraPos = float4(m_cameraPos, 1.0) * trans;
    m_focusPos = float4(m_focusPos, 1.0) * trans;

    updateProjViewMat();
}

void Camera::zoom(float delta)
{
    float zoomRatio = 1.f + delta / 10.f;
    float3 vecTargetCamera(m_cameraPos - m_focusPos);

    m_cameraPos = m_focusPos + vecTargetCamera * zoomRatio;

    updateProjViewMat();
}

void Camera::updateProjMat()
{
    m_projMat = float4x4::Projection(m_fov, m_aspectRatio, m_zNear, m_zFar, true);
}

void Camera::updateProjViewMat()
{
    // Classic lookAt function

    float3 refX, refY, refZ;

    refZ = -(m_cameraPos - m_focusPos);
    refZ = normalize(refZ);

    refY = { 0.0f, 1.0f, 0.0f };
    refX = cross(refY, refZ);
    refY = cross(refZ, refX);

    refX = normalize(refX);
    refY = normalize(refY);

    float dotRefXEye = -dot(refX, m_cameraPos);
    float dotRefYEye = -dot(refY, m_cameraPos);
    float dotRefZEye = -dot(refZ, m_cameraPos);

    m_viewMat = {
            refX.x, refY.x, refZ.x, 0.0f,
            refX.y, refY.y, refZ.y, 0.0f,
            refX.z, refY.z, refZ.z, 0.0f,
            dotRefXEye, dotRefYEye, dotRefZEye, 1.0f
    };

    m_projViewMat = float4x4::Mul(m_viewMat, m_projMat);
}
