
import * as React from 'react';
import RawCalendarView from './CalendarViewNativeComponent';

import XamlHost from 'react-native-windows/Libraries/Components/Xaml/XamlHost';

function CalendarView(props: any) {
  return (
    <XamlHost label='unused'>
      <RawCalendarView label='unused' {...props} />
    </XamlHost>
  );
}

export { CalendarView, RawCalendarView };
