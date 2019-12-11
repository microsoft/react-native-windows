import { ViewProps } from 'react-native';

export interface IImageSourceProps {
  uri?: string;
  method?: string;
  headers?: { [key: string]: string };
  body?: string;
  width?: number;
  height?: number;
  scale?: number;
}

export interface IImageProps extends ViewProps {
  // React.ClassAttributes<React.View>
  source?: string | IImageSourceProps;
  resizeMode?: 'cover' | 'contain' | 'stretch' | 'center';
  onLoad: () => void;
  onError: () => void;
}

export interface IImageState {}
