/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import QuirkStorage from './QuirkStorage';

const QuirkSettings = {
  // Enables a refactored VirtualizedList with better keyboarding behavior
  get enableFocusAwareRealization(): ?boolean {
    return QuirkStorage.enableFocusAwareRealization();
  },
};

export default QuirkSettings;
