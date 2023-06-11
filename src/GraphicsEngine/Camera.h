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
#include "Math.hpp"

namespace Render
{
    class Camera
    {
    public:
        Camera(float sceneAspectRatio);
        ~Camera() = default;

        void reset();

        inline void setSceneAspectRatio(float aspectRatio)
        {
            m_aspectRatio = aspectRatio;
            updateProjMat();
            updateProjViewMat();
        }

        inline bool isAutoRotating() const { return m_isAutoRotating; }
        inline void enableAutoRotating(bool autoRotate) { m_isAutoRotating = autoRotate; }

        void rotate(float angleX, float angleY);
        void translate(float dispX, float dispY);
        void zoom(float delta);

        inline const Math::float3 cameraPos() const { return m_cameraPos; }
        inline const Math::float3 focusPos() const { return m_focusPos; }

        inline Math::float4x4 getProjViewMat() const { return m_projViewMat; }

    private:
        void updateProjMat();
        void updateProjViewMat();

        Math::float3 m_cameraPos, m_cameraInitPos;
        Math::float3 m_focusPos, m_focusInitPos;
        Math::float4x4 m_projMat, m_viewMat, m_projViewMat;

        float m_fov;
        float m_aspectRatio;
        float m_zNear;
        float m_zFar;
        bool m_isAutoRotating;
    };
}