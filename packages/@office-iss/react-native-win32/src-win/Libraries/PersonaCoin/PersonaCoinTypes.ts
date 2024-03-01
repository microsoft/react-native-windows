import { ViewProps } from 'react-native';
import { IImageSourceProps } from '../Image/ImageTypes';

export enum PersonaCoinPresence {
  none,
  away,
  blocked,
  busy,
  doNotDisturb,
  offline,
  online,
  oof
}

export enum PersonaCoinSize {
  extraSmall,
  small,
  normal,
  large,
  extraLarge,
  extraExtraLarge
}

export interface IPersonaCoinProps extends ViewProps {
  source?: string | IImageSourceProps;
  brandIconSource?: string | IImageSourceProps;
  size: PersonaCoinSize;
  presence: PersonaCoinPresence;
  initials?: string;
  displayName?: string;
  emailAddress?: string;
  disabled?: boolean;
  onError?: () => void;
  onLoad?: () => void;
  color?: number;
}
