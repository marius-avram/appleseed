
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014 Francois Beaune, The appleseedhq Organization
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
#include "filteredtile.h"

// appleseed.foundation headers.
#include "foundation/image/pixel.h"
#include "foundation/math/scalar.h"
#include "foundation/math/vector.h"

// Standard headers.
#include <cmath>

using namespace std;

namespace foundation
{

//
// We use the discrete-to-continuous mapping described in:
//
//   Physically Based Rendering, first edition, p. 295
//
// Other references:
//
//   Physically Based Rendering, first edition, pp. 350-352 and pp. 371-377
//
//   A Pixel Is Not A Little Square, Technical Memo 6, Alvy Ray Smith
//   http://alvyray.com/Memos/CG/Microsoft/6_pixel.pdf
//

FilteredTile::FilteredTile(
    const size_t        width,
    const size_t        height,
    const size_t        channel_count,
    const Filter2d&     filter)
  : Tile(width, height, channel_count + 1, PixelFormatFloat)
  , m_crop_window(Vector2u(0, 0), Vector2u(width - 1, height - 1))
  , m_filter(filter)
{
}

FilteredTile::FilteredTile(
    const size_t        width,
    const size_t        height,
    const size_t        channel_count,
    const AABB2u&       crop_window,
    const Filter2d&     filter)
  : Tile(width, height, channel_count + 1, PixelFormatFloat)
  , m_crop_window(crop_window)
  , m_filter(filter)
{
}

void FilteredTile::clear()
{
    float* ptr = reinterpret_cast<float*>(pixel(0));

    for (size_t i = 0; i < m_pixel_count * m_channel_count; ++i)
        ptr[i] = 0.0f;
}

void FilteredTile::add(
    const double        x,
    const double        y,
    const float*        values)
{
    // Convert (x, y) from continuous image space to discrete image space.
    const double dx = x - 0.5;
    const double dy = y - 0.5;

    // Find the pixels affected by this sample.
    AABB2i footprint;
    footprint.min.x = truncate<int>(ceil(dx - m_filter.get_xradius()));
    footprint.min.y = truncate<int>(ceil(dy - m_filter.get_yradius()));
    footprint.max.x = truncate<int>(floor(dx + m_filter.get_xradius()));
    footprint.max.y = truncate<int>(floor(dy + m_filter.get_yradius()));

    // Don't affect pixels outside the crop window.
    footprint = AABB2i::intersect(footprint, m_crop_window);

    for (int ry = footprint.min.y; ry <= footprint.max.y; ++ry)
    {
        for (int rx = footprint.min.x; rx <= footprint.max.x; ++rx)
        {
            const float weight = static_cast<float>(m_filter.evaluate(rx - dx, ry - dy));

            float* RESTRICT ptr = reinterpret_cast<float*>(pixel(rx, ry));

            *ptr++ += weight;

            for (size_t i = 0; i < m_channel_count - 1; ++i)
                ptr[i] += values[i] * weight;
        }
    }
}

}   // namespace foundation
