import { IViewWin32Props } from '../View/ViewWin32.Props';

export interface ITouchableNativeFeedbackProps extends IViewWin32Props {
  // ViewProperties {
  onPress: (event: Object) => void;
  disabled?: boolean;
  accessibilityLabel: string;
  tooltip?: string;
}
