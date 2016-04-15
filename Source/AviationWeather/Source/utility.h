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

} // namespace aw
