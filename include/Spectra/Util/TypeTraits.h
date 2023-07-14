// Copyright (C) 2018-2022 Yixuan Qiu <yixuan.qiu@cos.name>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SPECTRA_TYPE_TRAITS_H
#define SPECTRA_TYPE_TRAITS_H

#include <Eigen/Core>
#include <limits>

/// \cond

// Clang-Format will have unintended effects:
//     static constexpr Scalar(min)()
// So we turn it off here
//
// clang-format off

namespace Spectra {

// For a real value type "Scalar", we want to know its smallest
// positive value, i.e., std::numeric_limits<Scalar>::min().
// However, we must take non-standard value types into account,
// so we rely on Eigen::NumTraits.
//
// Eigen::NumTraits has defined epsilon() and lowest(), but
// lowest() means negative highest(), which is a very small
// negative value.
//
// Therefore, we manually define this limit, and use eplison()^3
// to mimic it for non-standard types.

// Generic definition
template <typename Scalar>
struct TypeTraits
{
    const Scalar epsilon()
    {
        return Eigen::numext::numeric_limits<Scalar>::epsilon();
    }
    const Scalar (min)()
    {
        return epsilon() * epsilon() * epsilon();
    }
};

// Full specialization
template <>
struct TypeTraits<float>
{
    const float epsilon()
    {
        return std::numeric_limits<float>::epsilon();
    }
    const float (min)()
    {
        return (std::numeric_limits<float>::min)();
    }
};

template <>
struct TypeTraits<double>
{
    static constexpr double epsilon()
    {
        return std::numeric_limits<double>::epsilon();
    }
    static constexpr double (min)()
    {
        return (std::numeric_limits<double>::min)();
    }
};

template <>
struct TypeTraits<long double>
{
    const long double epsilon()
    {
        return std::numeric_limits<long double>::epsilon();
    }
    const long double (min)()
    {
        return (std::numeric_limits<long double>::min)();
    }
};

// Get the element type of a "scalar"
// ElemType<double>                 => double
// ElemType<std::complex<double>>   => double
template <typename T>
using ElemType = typename Eigen::NumTraits<T>::Real;

}  // namespace Spectra

/// \endcond

#endif  // SPECTRA_TYPE_TRAITS_H
