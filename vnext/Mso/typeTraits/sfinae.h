// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_TYPETRAITS_SFINAE_H
#define MSO_TYPETRAITS_SFINAE_H

//! Helper definitions to implement conditional template method compilation
//! based on SFINAE principles: https://en.cppreference.com/w/cpp/language/sfinae

#include <type_traits>

namespace Mso {

template <class TBase, class TDerived>
using EnableIfIsBaseOf = typename std::enable_if<std::is_base_of<TBase, TDerived>::value, int>::type;

} // namespace Mso

#endif // MSO_TYPETRAITS_SFINAE_H
