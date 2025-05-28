
import * as React from 'react';
import RawCalendarView from './CalendarViewNativeComponent';

import XamlHost from 'react-native-windows/Libraries/Components/Xaml/XamlHost';

function CalendarView(props: any) {
  return (
    <XamlHost {...props}>
      <RawCalendarView label='unused' />
    </XamlHost>
  );
}

export { CalendarView, RawCalendarView };
