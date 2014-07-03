
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2014 Marius Avram, The appleseedhq Organization
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
#include "disneymaterialcustomui.h"

// Qt headers.
#include <QPushButton>
#include <QVBoxLayout>

using namespace foundation;

namespace appleseed {
namespace studio {

DisneyMaterialCustomUI::DisneyMaterialCustomUI()
{

}

void DisneyMaterialCustomUI::create_custom_widgets(
    QVBoxLayout*        layout,
    const Dictionary&   values)
{
    // Create some example widgets, for testing.
    QPushButton *x = new QPushButton();
    x->setText("XXX");
    layout->addWidget(x);

    x = new QPushButton();
    x->setText("YYY");
    layout->addWidget(x);

    x = new QPushButton();
    x->setText("ZZZ");
    layout->addWidget(x);
}

Dictionary DisneyMaterialCustomUI::get_values() const
{
    return Dictionary();
}

}       // namespace studio
}       // namespace appleseed
