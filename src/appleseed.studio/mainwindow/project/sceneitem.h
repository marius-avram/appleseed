
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2012 Francois Beaune, Jupiter Jazz Limited
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef APPLESEED_STUDIO_MAINWINDOW_PROJECT_SCENEITEM_H
#define APPLESEED_STUDIO_MAINWINDOW_PROJECT_SCENEITEM_H

// appleseed.studio headers.
#include "mainwindow/project/basegroupitem.h"

// appleseed.foundation headers.
#include "foundation/platform/compiler.h"

// Forward declarations.
namespace appleseed { namespace studio { template <typename Entity, typename ParentEntity, typename ParentItem> class CollectionItem; } }
namespace appleseed { namespace studio { template <typename Entity, typename ParentEntity> class MultiModelEntityItem; } }
namespace appleseed { namespace studio { template <typename Entity, typename ParentEntity> class SingleModelEntityItem; } }
namespace appleseed { namespace studio { class ProjectBuilder; } }
namespace renderer  { class Camera; }
namespace renderer  { class Environment; }
namespace renderer  { class EnvironmentEDF; }
namespace renderer  { class EnvironmentShader; }
namespace renderer  { class ParamArray; }
namespace renderer  { class Scene; }
class QMenu;

namespace appleseed {
namespace studio {

class SceneItem
  : public BaseGroupItem
{
  public:
    SceneItem(
        renderer::Scene&                scene,
        ProjectBuilder&                 project_builder,
        renderer::ParamArray&           settings);

    virtual QMenu* get_single_item_context_menu() const override;

    void add_item(renderer::EnvironmentEDF* environment_edf);
    void add_item(renderer::EnvironmentShader* environment_shader);

  private:
    typedef MultiModelEntityItem<renderer::Camera, renderer::Scene> CameraItem;
    typedef SingleModelEntityItem<renderer::Environment, renderer::Scene> EnvironmentItem;
    typedef CollectionItem<renderer::EnvironmentEDF, renderer::Scene, SceneItem> EnvironmentEDFCollectionItem;
    typedef CollectionItem<renderer::EnvironmentShader, renderer::Scene, SceneItem> EnvironmentShaderCollectionItem;

    CameraItem*                         m_camera_item;
    EnvironmentItem*                    m_environment_item;
    EnvironmentEDFCollectionItem*       m_environment_edf_collection_item;
    EnvironmentShaderCollectionItem*    m_environment_shader_collection_item;
};

}       // namespace studio
}       // namespace appleseed

#endif  // !APPLESEED_STUDIO_MAINWINDOW_PROJECT_SCENEITEM_H