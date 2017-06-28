using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ReactNative.Modules.NetInfo
{
    internal static class NetInfoModuleNativeMethods
    {
        /// <summary>
        /// Retrieves the connected state of the local system.
        /// </summary>
        /// <param name="lpdwFlags">Pointer to a variable that receives the connection description. This parameter may return a valid flag even when the function returns FALSE. This parameter can be one or more of the following values.</param>
        /// <param name="dwReserved"></param>
        /// <returns></returns>
        [DllImport("wininet.dll", SetLastError = true)]
        public static extern bool InternetGetConnectedState(out int lpdwFlags, int dwReserved);

        [Flags]
        enum ConnectionStates
        {
            INTERNET_CONNECTION_MODEM = 0x1, // Local system uses a modem to connect to the Internet.
            INTERNET_CONNECTION_LAN = 0x2, // Local system uses a local area network to connect to the Internet.
            INTERNET_CONNECTION_PROXY = 0x4, // Local system uses a proxy server to connect to the Internet.
            INTERNET_RAS_INSTALLED = 0x10, // Local system has RAS installed.
            INTERNET_CONNECTION_OFFLINE = 0x20, // Local system is in offline mode.
            INTERNET_CONNECTION_CONFIGURED = 0x40, // Local system has a valid connection to the Internet, but it might or might not be currently connected.
        }
    }
}
