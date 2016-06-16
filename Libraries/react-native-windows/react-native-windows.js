/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @noflow - get/set properties not yet supported by flow. also `...require(x)` is broken #6560135
 */
'use strict';

var ReactWindows = {
  // Components
  get FlipViewWindows() { return require('FlipViewWindows'); },
  get ProgressBarWindows() {return require('ProgressBarWindows');},
  get ProgressRingWindows() {return require('ProgressRingWindows');},
  get SplitViewWindows() { return require('SplitViewWindows'); },
};

module.exports = ReactWindows;
