// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MSO_TYPETRAITS_TAGS_H
#define MSO_TYPETRAITS_TAGS_H

namespace Mso {

struct AttachTagType {
  constexpr AttachTagType() noexcept {}
};

constexpr AttachTagType AttachTag;

} // namespace Mso

#endif // MSO_TYPETRAITS_TAGS_H
