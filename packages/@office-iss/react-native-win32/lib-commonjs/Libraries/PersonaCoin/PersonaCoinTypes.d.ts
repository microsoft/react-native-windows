import { ViewProps } from 'react-native';
import { IImageSourceProps } from '../Image/ImageTypes';
export declare enum PersonaCoinPresence {
    none = 0,
    away = 1,
    blocked = 2,
    busy = 3,
    doNotDisturb = 4,
    offline = 5,
    online = 6,
    oof = 7
}
export declare enum PersonaCoinSize {
    extraSmall = 0,
    small = 1,
    normal = 2,
    large = 3,
    extraLarge = 4,
    extraExtraLarge = 5
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
