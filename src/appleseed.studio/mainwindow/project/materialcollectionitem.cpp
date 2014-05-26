
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2014 Esteban Tovagliari, The appleseedhq Organization
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

// Interface header.
#include "materialcollectionitem.h"

// appleseed.studio headers.
#include "mainwindow/project/assemblyitem.h"
#include "mainwindow/project/itemregistry.h"
#include "mainwindow/project/materialitem.h"
#include "mainwindow/project/projectbuilder.h"
#include "mainwindow/rendering/renderingmanager.h"
#include "utility/interop.h"
#include "utility/settingskeys.h"

// appleseed.renderer headers.
#include "renderer/api/utility.h"

// appleseed.foundation headers.
#include "foundation/math/transform.h"
#include "foundation/utility/containers/dictionary.h"
#include "foundation/utility/autoreleaseptr.h"
#include "foundation/utility/searchpaths.h"
#include "foundation/utility/uid.h"

// Standard headers.
#include <cassert>
#include <cstddef>

using namespace foundation;
using namespace renderer;
using namespace std;

namespace appleseed {
namespace studio {

namespace
{
    const UniqueID g_class_uid = new_guid();
}

MaterialCollectionItem::MaterialCollectionItem(
    MaterialContainer&  materials,
    Assembly&           parent,
    AssemblyItem*       parent_item,
    ProjectBuilder&     project_builder,
    ParamArray&         settings)
  : CollectionItemBase<Material>(g_class_uid, "Materials", project_builder)
  , m_parent(parent)
  , m_parent_item(parent_item)
  , m_settings(settings)
{
    add_items(materials);
}

ItemBase* MaterialCollectionItem::create_item(Material* material)
{
    assert(material);

    ItemBase* item =
        new MaterialItem(
            material,
            m_parent,
            m_parent_item,
            m_project_builder);

    m_project_builder.get_item_registry().insert(material->get_uid(), item);

    return item;
}

}   // namespace studio
}   // namespace appleseed
