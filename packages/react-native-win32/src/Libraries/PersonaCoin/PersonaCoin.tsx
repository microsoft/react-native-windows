'use strict';
import * as React from 'react';

import { requireNativeComponent } from 'react-native';
import { IPersonaCoinProps } from './PersonaCoinTypes';
import { PersonaCoinDefaultProps } from './PersonaCoinPropTypes';

const RCTPersonaCoin = requireNativeComponent('RCTPersonaCoin');

const PersonaCoin: React.FunctionComponent<IPersonaCoinProps> = (props: IPersonaCoinProps) => <RCTPersonaCoin {...props} />;
PersonaCoin.defaultProps = PersonaCoinDefaultProps;
PersonaCoin.displayName = 'PersonaCoin';

export default PersonaCoin;
