/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import {IKeyboardEvent, IHandledKeyboardEvent} from '../View/ViewPropTypes';

/**
 * @deprecated
 */
export interface IKeyboardProps {
  onKeyDown?: (args: IKeyboardEvent) => void;
  onKeyDownCapture?: (args: IKeyboardEvent) => void;
  onKeyUp?: (args: IKeyboardEvent) => void;
  onKeyUpCapture?: (args: IKeyboardEvent) => void;

  keyDownEvents?: IHandledKeyboardEvent[];
  keyUpEvents?: IHandledKeyboardEvent[];
}
