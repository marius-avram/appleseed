
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014 Francois Beaune, The appleseedhq Organization
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

#ifndef APPLESEED_FOUNDATION_MATH_MICROFACET2_H
#define APPLESEED_FOUNDATION_MATH_MICROFACET2_H

// appleseed.foundation headers.
#include "foundation/core/concepts/noncopyable.h"
#include "foundation/math/scalar.h"
#include "foundation/math/vector.h"
#include "foundation/platform/compiler.h"

// boost headers.
#include "boost/mpl/bool.hpp"

// Standard headers.
#include <algorithm>
#include <cassert>
#include <cmath>

namespace foundation
{

//
// Torrance-Sparrow Masking Shadowing Function.
//
// References:
//
//   [1] Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs
//       http://hal.inria.fr/docs/00/96/78/44/PDF/RR-8468.pdf
//
//   [2] Importance Sampling Microfacet-Based BSDFs using the Distribution of Visible Normals
//       http://hal.inria.fr/docs/01/00/66/20/PDF/article.pdf
//

template <typename T>
class TorranceSparrowMaskingShadowing
{
  public:
    TorranceSparrowMaskingShadowing() {}

    // incoming and outgoing are in shading basis space.
    static T G(
        const Vector<T, 3>&  incoming,
        const Vector<T, 3>&  outgoing,
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y)
    {
        if (incoming.y >= T(0.0))
            return std::min(G1(incoming, h), G1(outgoing, h));

        return std::max(G1(incoming, h) + G1(outgoing, h) - T(1.0), T(0.0));
    }

  private:
    static T G1(
        const Vector<T, 3>&  v,
        const Vector<T, 3>&  h)
    {
        if (v.y <= T(0.0))
            return T(0.0);

        if (dot(v, h) <= T(0.0))
            return T(0.0);
        
        const T cos_vh = std::abs(dot(v, h));
        if (cos_vh == T(0.0))
            return T(0.0);

        return std::min(T(1.0), T(2.0) * std::abs(h.y) * std::abs(v.y) / cos_vh);
    }
};

template <typename T>
class MDF
  : public NonCopyable
{
  public:
    typedef T ValueType;

    MDF() {}

    virtual ~MDF() {}

    Vector<T, 3> sample(
        const Vector<T, 2>&  s,
        const T              alpha_x,
        const T              alpha_y) const
    {
        // Preconditions.
        assert(s[0] >= T(0.0));
        assert(s[0] <  T(1.0));
        assert(s[1] >= T(0.0));
        assert(s[1] <  T(1.0));

        const Vector<T, 3> result = do_sample(s, alpha_x, alpha_y);

        // Postconditions.
        assert(is_normalized(result));
        return result;
    }

    T D(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const
    {
        // Preconditions.
        assert(cos_theta(h) >= T(0.0));

        const T result = do_eval_D(h, alpha_x, alpha_y);

        // Postconditions.
        assert(result >= T(0.0));
        return result;
    }

    T G(
        const Vector<T, 3>&  incoming,
        const Vector<T, 3>&  outgoing,
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const
    {
        const T result = do_eval_G(incoming, outgoing, h, alpha_x, alpha_y);

        // Postconditions.
        assert(result >= T(0.0));
        return result;
    }

    T pdf(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const
    {
        // Preconditions.
        assert(cos_theta(h) >= T(0.0));

        const T result = do_eval_pdf(h, alpha_x, alpha_y);

        // Postconditions.
        assert(result >= T(0.0));
        return result;
    }

  protected:
    T cos_theta(const Vector<T, 3>& v) const
    {
        return v.y;
    }

    T sin_theta_2(const Vector<T, 3>& v) const
    {
        return T(1.0) - square(cos_theta(v));
    }

    T sin_theta(const Vector<T, 3>& v) const
    {
        return std::sqrt(std::max(T(0.0), sin_theta_2(v)));
    }

    T cos_phi(const Vector<T, 3>& v) const
    {
        return v.x / sin_theta(v);
    }

    T sin_phi(const Vector<T, 3>& v) const
    {
        return v.z / sin_theta(v);
    }

  private:
    virtual Vector<T, 3> do_sample(
        const Vector<T, 2>&  s,
        const T              alpha_x,
        const T              alpha_y) const = 0;

    // h is in shading basis space.
    virtual T do_eval_D(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const = 0;

    // By default, TorranceSparrow geometric attenuation function.
    // incoming and outgoing are in shading basis space.
    virtual T do_eval_G(
        const Vector<T, 3>&  incoming,
        const Vector<T, 3>&  outgoing,
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const
    {
        return
            TorranceSparrowMaskingShadowing<T>::G(
                incoming,
                outgoing,
                h,
                alpha_x,
                alpha_y);
    }

    // h is in shading basis space.
    virtual T do_eval_pdf(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const = 0;
};


//
// Blinn-Phong Microfacet Distribution Function.
//
// References:
//
//   Physically Based Rendering, first edition, pp. 444-447 and 681-684
//
//   Microfacet Models for Refraction through Rough Surfaces
//   http://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
//

template <typename T>
class BlinnMDF2
  : public MDF<T>
{
  public:
    typedef boost::mpl::bool_<false> IsAnisotropicType;

    BlinnMDF2() {}

  private:
    virtual Vector<T, 3> do_sample(
        const Vector<T, 2>&  s,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        const T cos_theta = std::pow(T(1.0) - s[0], T(1.0) / (alpha_x + T(2.0)));
        const T sin_theta = std::sqrt(T(1.0) - cos_theta * cos_theta);
        const T phi = T(TwoPi) * s[1];
        return Vector<T, 3>::unit_vector(cos_theta, sin_theta, std::cos(phi), std::sin(phi));
    }

    virtual T do_eval_D(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        return (alpha_x + T(2.0)) * T(RcpTwoPi) * std::pow(this->cos_theta(h), alpha_x);
    }

    virtual T do_eval_pdf(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        return (alpha_x + T(2.0)) * T(RcpTwoPi) * std::pow(this->cos_theta(h), alpha_x + T(1.0));
    }
};


//
// Isotropic Beckmann Microfacet Distribution Function.
//
// References:
//
//   [1] http://en.wikipedia.org/wiki/Specular_highlight#Beckmann_distribution
//
//   [2] A Microfacet Based Coupled Specular-Matte BRDF Model with Importance Sampling
//       http://sirkan.iit.bme.hu/~szirmay/scook.pdf
//
//   [3] Microfacet Models for Refraction through Rough Surfaces
//       http://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
//
//   [4] Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs
//       http://hal.inria.fr/docs/00/96/78/44/PDF/RR-8468.pdf
//
//

template <typename T>
class BeckmannSmithMaskingShadowing
{
  public:
    BeckmannSmithMaskingShadowing() {}

    // incoming and outgoing are in shading basis space.
    static T G(
        const Vector<T, 3>&  incoming,
        const Vector<T, 3>&  outgoing,
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y)
    {
        return G1(outgoing, h, alpha_x, alpha_y) * G1(incoming, h, alpha_x, alpha_y);
    }

    // G1 is used in microfacet2 unit tests, so it's public.
    static T G1(
        const Vector<T, 3>&  v,
        const Vector<T, 3>&  m,
        const T              alpha_x,
        const T              alpha_y)
    {
        if (dot(v, m) * v.y <= T(0.0))
            return T(0.0);

        const T cos_theta_2 = square(v.y);
        const T tan_theta = std::sqrt((T(1.0) - cos_theta_2) / cos_theta_2);
        
        if (tan_theta == T(0.0))
            return T(1.0);

        const T a = T(1.0) / alpha_x * tan_theta;

        if (a < T(1.6))
        {
            const T a2 = square(a);
            return (T(3.535) * a + T(2.181) * a2) / (T(1.0) + T(2.276) * a + T(2.577) * a2);
        }

        return T(1.0);
    }
};

template <typename T, template <typename> class MaskingShadowingFunction = BeckmannSmithMaskingShadowing>
class BeckmannMDF2
  : public MDF<T>
{
  public:
    typedef boost::mpl::bool_<false> IsAnisotropicType;

    BeckmannMDF2() {}

  private:
    virtual Vector<T, 3> do_sample(
        const Vector<T, 2>&  s,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        // Same sampling procedure as for the Ward distribution.
        const T alpha_x_2 = square(alpha_x);
        const T tan_theta_2 = alpha_x_2 * (-std::log(T(1.0) - s[0]));
        const T cos_theta = T(1.0) / std::sqrt(T(1.0) + tan_theta_2);
        const T sin_theta = cos_theta * std::sqrt(tan_theta_2);
        const T phi = T(TwoPi) * s[1];
        return Vector<T, 3>::unit_vector(cos_theta, sin_theta, std::cos(phi), std::sin(phi));
    }

    virtual T do_eval_D(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        if (this->cos_theta(h) == T(0.0))
            return T(0.0);

        const T cos_theta_2 = square(this->cos_theta(h));
        const T cos_theta_4 = square(cos_theta_2);
        const T tan_theta_2 = (T(1.0) - cos_theta_2) / cos_theta_2;
        const T alpha_x_2 = square(alpha_x);

        // Note: in [2] there's a missing Pi factor in the denominator.
        return std::exp(-tan_theta_2 / alpha_x_2) / (alpha_x_2 * T(Pi) * cos_theta_4);
    }

    virtual T do_eval_G(
        const Vector<T, 3>&  incoming,
        const Vector<T, 3>&  outgoing,
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        return
            MaskingShadowingFunction<T>::G(
                incoming,
                outgoing,
                h,
                alpha_x,
                alpha_y);
    }

    virtual T do_eval_pdf(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        if (this->cos_theta(h) == T(0.0))
            return T(0.0);

        const T cos_theta_2 = square(this->cos_theta(h));
        const T cos_theta_3 = this->cos_theta(h) * cos_theta_2;
        const T tan_theta_2 = (T(1.0) - cos_theta_2) / cos_theta_2;
        const T alpha_x_2 = square(alpha_x);
        return std::exp(-tan_theta_2 / alpha_x_2) / (alpha_x_2 * T(Pi) * cos_theta_3);
    }
};


//
// Anisotropic GGX Microfacet Distribution Function.
//
// References:
//
//   [1] Microfacet Models for Refraction through Rough Surfaces
//       http://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
//
//   [2] Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs
//       http://hal.inria.fr/docs/00/96/78/44/PDF/RR-8468.pdf
//

template <typename T>
class GGXSmithMaskingShadowing
{
  public:
    GGXSmithMaskingShadowing() {}

    // incoming and outgoing are in shading basis space.
    static T G(
        const Vector<T, 3>&  incoming,
        const Vector<T, 3>&  outgoing,
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y)
    {
        return G1(outgoing, h, alpha_x, alpha_y) * G1(incoming, h, alpha_x, alpha_y);
    }

    // G1 is used in microfacet2 unit tests, so it's public.
    static T G1(
        const Vector<T, 3>&  v,
        const Vector<T, 3>&  m,
        const T              alpha_x,
        const T              alpha_y)
    {
        if (dot(v, m) * v.y <= T(0.0))
            return T(0.0);

        const T cos_theta = std::abs(v.y);

        if (cos_theta == T(1.0))
            return T(0.0);

        const T cos_theta_2 = square(cos_theta);

        if (alpha_x != alpha_y)
        {
            // [2] page 15.
            const T sin_theta = std::sqrt(std::max(T(1.0) - cos_theta_2, T(0.0)));
            const T cos_phi_2 = square(v.x / sin_theta);
            const T sin_phi_2 = square(v.z / sin_theta);
            const T alpha = std::sqrt(cos_phi_2 * square(alpha_x) + sin_phi_2 * square(alpha_y));
            const T a = cos_theta / (alpha * sin_theta);
            const T lambda = (T(-1) + std::sqrt(T(1.0) + T(1.0) / square(a))) * T(0.5);
            return T(1.0) / (T(1.0) + lambda);
        }

        // [2] page 13.
        const T tan_theta_2 = (T(1.0) - cos_theta_2) / cos_theta_2;
        const T a2_rcp = square(alpha_x) * tan_theta_2;
        const T A = (T(-1.0) + std::sqrt(T(1.0) + a2_rcp)) * T(0.5);
        return T(1.0) / (T(1.0) + A);
    }
};

template <typename T, template <typename> class MaskingShadowingFunction = GGXSmithMaskingShadowing>
class GGXMDF2
  : public MDF<T>
{
  public:
    typedef boost::mpl::bool_<true> IsAnisotropicType;

    GGXMDF2() {}

  private:
    virtual Vector<T, 3> do_sample(
        const Vector<T, 2>&  s,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        T cos_phi, sin_phi, tan_theta_2;

        if (alpha_x != alpha_y)
        {
            Vector<T, 2> sin_cos_phi(
                std::cos(T(TwoPi) * s[1]) * alpha_x,
                std::sin(T(TwoPi) * s[1]) * alpha_y);
            sin_cos_phi = normalize(sin_cos_phi);
            cos_phi = sin_cos_phi[0];
            sin_phi = sin_cos_phi[1];
            const T tmp = square(cos_phi / alpha_x) + square(sin_phi / alpha_y);
            tan_theta_2 = s[0] / ((T(1.0) - s[0]) * tmp);
        }
        else
        {
            tan_theta_2 = square(alpha_x) * s[0] / (T(1.0) - s[0]);
            const T phi = T(TwoPi) * s[1];
            cos_phi = std::cos(phi);
            sin_phi = std::sin(phi);
        }

        const T cos_theta = T(1.0) / std::sqrt(T(1.0) + tan_theta_2);
        const T sin_theta = std::sqrt(T(1.0) - square(cos_theta));
        return Vector<T, 3>::unit_vector(cos_theta, sin_theta, cos_phi, sin_phi);
    }

    virtual T do_eval_D(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        const T alpha_x_2 = square(alpha_x);

        const T cos_theta = this->cos_theta(h);

        if (cos_theta == T(0.0))
            return alpha_x_2 * T(RcpPi);

        const T cos_theta_2 = square(cos_theta);
        const T cos_theta_4 = square(cos_theta_2);

        if (alpha_x != alpha_y)
        {
            const T sin_theta = this->sin_theta(h);

            T cos_phi_2_ax_2;
            T sin_phi_2_ay_2;

            if (sin_theta != T(0.0))
            {
                cos_phi_2_ax_2 = square(h.x / sin_theta) / alpha_x_2;
                sin_phi_2_ay_2 = square(h.z / sin_theta) / square(alpha_y);
            }
            else
            {
                // Choose some arbitrary phi angle (0).
                cos_phi_2_ax_2 = T(1.0) / alpha_x_2;
                sin_phi_2_ay_2 = T(0.0);
            }

            const T tan_theta_2 = square(sin_theta) / cos_theta_2;
            const T tmp = T(1.0) + tan_theta_2 * (cos_phi_2_ax_2 + sin_phi_2_ay_2);
            return T(1.0) / (T(Pi) * alpha_x * alpha_y * cos_theta_4 * square(tmp));
        }

        const T tan_theta_2 = (T(1.0) - cos_theta_2) / cos_theta_2;
        return alpha_x_2 / (T(Pi) * cos_theta_4 * square(alpha_x_2 + tan_theta_2));
    }

    virtual T do_eval_G(
        const Vector<T, 3>&  incoming,
        const Vector<T, 3>&  outgoing,
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        return
            MaskingShadowingFunction<T>::G(
                incoming,
                outgoing,
                h,
                alpha_x,
                alpha_y);
    }

    virtual T do_eval_pdf(
        const Vector<T, 3>&  h,
        const T              alpha_x,
        const T              alpha_y) const OVERRIDE
    {
        if (this->cos_theta(h) == T(0.0))
            return T(0.0);

        const T alpha_x_2 = square(alpha_x);
        const T cos_theta_2 = square(this->cos_theta(h));
        const T cos_theta_3 = h.y * cos_theta_2;

        if (alpha_x != alpha_y)
        {
            const T sin_theta = this->sin_theta(h);
            T cos_phi_2_ax_2;
            T sin_phi_2_ay_2;

            if (sin_theta != T(0.0))
            {
                cos_phi_2_ax_2 = square(h.x / sin_theta) / alpha_x_2;
                sin_phi_2_ay_2 = square(h.z / sin_theta) / square(alpha_y);
            }
            else
            {
                // Choose some arbitrary phi angle (0).
                cos_phi_2_ax_2 = T(1.0) / alpha_x_2;
                sin_phi_2_ay_2 = T(0.0);
            }

            const T tan_theta_2 = square(sin_theta) / cos_theta_2;
            const T tmp = T(1.0) + tan_theta_2 * (cos_phi_2_ax_2 + sin_phi_2_ay_2);
            return T(1.0) / (T(Pi) * alpha_x * alpha_y * cos_theta_3 * square(tmp));
        }

        const T tan_theta_2 = (T(1.0) - cos_theta_2) / cos_theta_2;
        return alpha_x_2 / (T(Pi) * cos_theta_3 * square(alpha_x_2 + tan_theta_2));
    }
};

}       // namespace foundation

#endif  // !APPLESEED_FOUNDATION_MATH_MICROFACET2_H
