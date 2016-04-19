/**********************************************************************************
 *                                                                                *
 * Copyright (c) 2015 Steven Frost, Orion Lyau. All rights reserved.              *
 *                                                                                *
 * This source is subject to the MIT License.                                     *
 * See http://opensource.org/licenses/MIT                                         *
 *                                                                                *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,    *
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED          *
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.         *
 *                                                                                *
 * NOT TO BE USED AS A SOLE SOURCE OF INFORMATION FOR FLIGHT CRITICAL OPERATIONS. *
 *                                                                                *
 **********************************************************************************/

#pragma once

#include <cmath>

namespace aw
{

//-----------------------------------------------------------------------------

template <class TExpected = double, class TEnum, class TUnit, class TVal, class TLambda>
bool comparison_conversion_helper(TEnum to, TUnit leftUnit, TUnit rightUnit, TVal const& leftVal, TVal const& rightVal, TLambda && l)
{
    auto lhs = leftUnit == to ? static_cast<TExpected>(leftVal) : convert<TExpected>(leftVal, leftUnit, to);
    auto rhs = rightUnit == to ? static_cast<TExpected>(rightVal) : convert<TExpected>(rightVal, rightUnit, to);

    return l(std::forward<TExpected>(lhs), std::forward<TExpected>(rhs));
}

//-----------------------------------------------------------------------------

template <class T>
bool are_approximately_equal(T a, T b)
{
    auto A = std::abs(a);
    auto B = std::abs(b);

    return std::abs(a - b) <= ((A < B ? B : A) * std::numeric_limits<T>::epsilon());
}

template <class T>
bool is_greater_than(T a, T b)
{
    auto A = std::abs(a);
    auto B = std::abs(b);

    return (a - b) > ((A < B ? B : A) * std::numeric_limits<T>::epsilon());
}

template <class T>
bool is_greater_than_or_equal(T a, T b)
{
    auto A = std::abs(a);
    auto B = std::abs(b);
    auto X = (A < B ? B : A);

    return (a - b) > (X * std::numeric_limits<T>::epsilon()) ||
        std::abs(a - b) <= (X * std::numeric_limits<T>::epsilon());
}

template <class T>
bool is_less_than(T a, T b)
{
    auto A = std::abs(a);
    auto B = std::abs(b);

    return (b - a) > ((A < B ? B : A) * std::numeric_limits<T>::epsilon());
}

template <class T>
bool is_less_than_or_equal(T a, T b)
{
    auto A = std::abs(a);
    auto B = std::abs(b);
    auto X = (A < B ? B : A);

    return (b - a) >(X * std::numeric_limits<T>::epsilon()) ||
        std::abs(a - b) <= (X * std::numeric_limits<T>::epsilon());
}

//-----------------------------------------------------------------------------

} // namespace aw
