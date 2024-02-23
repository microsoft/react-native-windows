import { IPersonaCoinProps, PersonaCoinPresence, PersonaCoinSize } from './PersonaCoinTypes';

export const PersonaCoinDefaultProps: IPersonaCoinProps = {
  source: {},
  brandIconSource: undefined,
  size: PersonaCoinSize.normal,
  presence: PersonaCoinPresence.none,
  initials: '',
  accessible: true,
  accessibilityLabel: undefined,
  displayName: '',
  emailAddress: '',
  disabled: false,
  onError: undefined,
  onLoad: undefined,
  color: undefined
};
