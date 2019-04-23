/* Header file automatically generated from ReactControl.g.idl */
/*
 * File built with Microsoft(R) MIDLRT Compiler Engine Version 10.00.0223 
 */

#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include <rpc.h>
#include <rpcndr.h>

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include <windows.h>
#include <ole2.h>
#endif /*COM_NO_WINDOWS_H*/
#ifndef __ReactControl2Eg_h__
#define __ReactControl2Eg_h__
#ifndef __ReactControl2Eg_p_h__
#define __ReactControl2Eg_p_h__


#pragma once

// Ensure that the setting of the /ns_prefix command line switch is consistent for all headers.
// If you get an error from the compiler indicating "warning C4005: 'CHECK_NS_PREFIX_STATE': macro redefinition", this
// indicates that you have included two different headers with different settings for the /ns_prefix MIDL command line switch
#if !defined(DISABLE_NS_PREFIX_CHECKS)
#define CHECK_NS_PREFIX_STATE "always"
#endif // !defined(DISABLE_NS_PREFIX_CHECKS)


#pragma push_macro("MIDL_CONST_ID")
#undef MIDL_CONST_ID
#define MIDL_CONST_ID const __declspec(selectany)


//  API Contract Inclusion Definitions
#if !defined(SPECIFIC_API_CONTRACT_DEFINITIONS)
#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATEDEVENTSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATEDEVENTSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATEDEVENTSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATIONCAMERASETTINGSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATIONCAMERASETTINGSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_ACTIVATIONCAMERASETTINGSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_CONTACTACTIVATEDEVENTSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_CONTACTACTIVATEDEVENTSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_CONTACTACTIVATEDEVENTSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_WEBUISEARCHACTIVATEDEVENTSCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_ACTIVATION_WEBUISEARCHACTIVATEDEVENTSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_ACTIVATION_WEBUISEARCHACTIVATEDEVENTSCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_BACKGROUND_BACKGROUNDALARMAPPLICATIONCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_BACKGROUND_BACKGROUNDALARMAPPLICATIONCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_BACKGROUND_BACKGROUNDALARMAPPLICATIONCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_BACKGROUND_CALLSBACKGROUNDCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_BACKGROUND_CALLSBACKGROUNDCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_BACKGROUND_CALLSBACKGROUNDCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSPHONECONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSVOIPCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_CALLSVOIPCONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_CALLSVOIPCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_CALLS_LOCKSCREENCALLCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_CALLS_LOCKSCREENCALLCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_CALLS_LOCKSCREENCALLCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_COMMUNICATIONBLOCKING_COMMUNICATIONBLOCKINGCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_COMMUNICATIONBLOCKING_COMMUNICATIONBLOCKINGCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_APPLICATIONMODEL_COMMUNICATIONBLOCKING_COMMUNICATIONBLOCKINGCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_FULLTRUSTAPPCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_FULLTRUSTAPPCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_FULLTRUSTAPPCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_SEARCH_SEARCHCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_SEARCH_SEARCHCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_SEARCH_SEARCHCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_STARTUPTASKCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_STARTUPTASKCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_APPLICATIONMODEL_STARTUPTASKCONTRACT_VERSION)

#if !defined(WINDOWS_APPLICATIONMODEL_WALLET_WALLETCONTRACT_VERSION)
#define WINDOWS_APPLICATIONMODEL_WALLET_WALLETCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_APPLICATIONMODEL_WALLET_WALLETCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_PRINTERS_EXTENSIONS_EXTENSIONSCONTRACT_VERSION)
#define WINDOWS_DEVICES_PRINTERS_EXTENSIONS_EXTENSIONSCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_DEVICES_PRINTERS_EXTENSIONS_EXTENSIONSCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDBACKGROUNDTRIGGERCONTRACT_VERSION)
#define WINDOWS_DEVICES_SMARTCARDS_SMARTCARDBACKGROUNDTRIGGERCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDBACKGROUNDTRIGGERCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDEMULATORCONTRACT_VERSION)
#define WINDOWS_DEVICES_SMARTCARDS_SMARTCARDEMULATORCONTRACT_VERSION 0x60000
#endif // defined(WINDOWS_DEVICES_SMARTCARDS_SMARTCARDEMULATORCONTRACT_VERSION)

#if !defined(WINDOWS_DEVICES_SMS_LEGACYSMSAPICONTRACT_VERSION)
#define WINDOWS_DEVICES_SMS_LEGACYSMSAPICONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_DEVICES_SMS_LEGACYSMSAPICONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)
#define WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_FOUNDATION_FOUNDATIONCONTRACT_VERSION)

#if !defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)
#define WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION 0x70000
#endif // defined(WINDOWS_FOUNDATION_UNIVERSALAPICONTRACT_VERSION)

#if !defined(WINDOWS_GAMING_INPUT_GAMINGINPUTPREVIEWCONTRACT_VERSION)
#define WINDOWS_GAMING_INPUT_GAMINGINPUTPREVIEWCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_GAMING_INPUT_GAMINGINPUTPREVIEWCONTRACT_VERSION)

#if !defined(WINDOWS_GLOBALIZATION_GLOBALIZATIONJAPANESEPHONETICANALYZERCONTRACT_VERSION)
#define WINDOWS_GLOBALIZATION_GLOBALIZATIONJAPANESEPHONETICANALYZERCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_GLOBALIZATION_GLOBALIZATIONJAPANESEPHONETICANALYZERCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_APPBROADCASTCONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_APPBROADCASTCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_MEDIA_CAPTURE_APPBROADCASTCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_APPCAPTURECONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_APPCAPTURECONTRACT_VERSION 0x40000
#endif // defined(WINDOWS_MEDIA_CAPTURE_APPCAPTURECONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_APPCAPTUREMETADATACONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_APPCAPTUREMETADATACONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_CAPTURE_APPCAPTUREMETADATACONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_CAMERACAPTUREUICONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_CAMERACAPTUREUICONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_CAPTURE_CAMERACAPTUREUICONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_CAPTURE_GAMEBARCONTRACT_VERSION)
#define WINDOWS_MEDIA_CAPTURE_GAMEBARCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_CAPTURE_GAMEBARCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_DEVICES_CALLCONTROLCONTRACT_VERSION)
#define WINDOWS_MEDIA_DEVICES_CALLCONTROLCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_DEVICES_CALLCONTROLCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_MEDIACONTROLCONTRACT_VERSION)
#define WINDOWS_MEDIA_MEDIACONTROLCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_MEDIACONTROLCONTRACT_VERSION)

#if !defined(WINDOWS_MEDIA_PROTECTION_PROTECTIONRENEWALCONTRACT_VERSION)
#define WINDOWS_MEDIA_PROTECTION_PROTECTIONRENEWALCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_MEDIA_PROTECTION_PROTECTIONRENEWALCONTRACT_VERSION)

#if !defined(WINDOWS_NETWORKING_CONNECTIVITY_WWANCONTRACT_VERSION)
#define WINDOWS_NETWORKING_CONNECTIVITY_WWANCONTRACT_VERSION 0x20000
#endif // defined(WINDOWS_NETWORKING_CONNECTIVITY_WWANCONTRACT_VERSION)

#if !defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)
#define WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_NETWORKING_SOCKETS_CONTROLCHANNELTRIGGERCONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)
#define WINDOWS_PHONE_PHONECONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONECONTRACT_VERSION)

#if !defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)
#define WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_PHONE_PHONEINTERNALCONTRACT_VERSION)

#if !defined(WINDOWS_SECURITY_ENTERPRISEDATA_ENTERPRISEDATACONTRACT_VERSION)
#define WINDOWS_SECURITY_ENTERPRISEDATA_ENTERPRISEDATACONTRACT_VERSION 0x50000
#endif // defined(WINDOWS_SECURITY_ENTERPRISEDATA_ENTERPRISEDATACONTRACT_VERSION)

#if !defined(WINDOWS_STORAGE_PROVIDER_CLOUDFILESCONTRACT_VERSION)
#define WINDOWS_STORAGE_PROVIDER_CLOUDFILESCONTRACT_VERSION 0x30000
#endif // defined(WINDOWS_STORAGE_PROVIDER_CLOUDFILESCONTRACT_VERSION)

#if !defined(WINDOWS_SYSTEM_ANDROMEDAPLACEHOLDERCONTRACT_VERSION)
#define WINDOWS_SYSTEM_ANDROMEDAPLACEHOLDERCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_SYSTEM_ANDROMEDAPLACEHOLDERCONTRACT_VERSION)

#if !defined(WINDOWS_SYSTEM_SYSTEMMANAGEMENTCONTRACT_VERSION)
#define WINDOWS_SYSTEM_SYSTEMMANAGEMENTCONTRACT_VERSION 0x60000
#endif // defined(WINDOWS_SYSTEM_SYSTEMMANAGEMENTCONTRACT_VERSION)

#if !defined(WINDOWS_UI_CORE_COREWINDOWDIALOGSCONTRACT_VERSION)
#define WINDOWS_UI_CORE_COREWINDOWDIALOGSCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_UI_CORE_COREWINDOWDIALOGSCONTRACT_VERSION)

#if !defined(WINDOWS_UI_VIEWMANAGEMENT_VIEWMANAGEMENTVIEWSCALINGCONTRACT_VERSION)
#define WINDOWS_UI_VIEWMANAGEMENT_VIEWMANAGEMENTVIEWSCALINGCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_UI_VIEWMANAGEMENT_VIEWMANAGEMENTVIEWSCALINGCONTRACT_VERSION)

#if !defined(WINDOWS_UI_WEBUI_CORE_WEBUICOMMANDBARCONTRACT_VERSION)
#define WINDOWS_UI_WEBUI_CORE_WEBUICOMMANDBARCONTRACT_VERSION 0x10000
#endif // defined(WINDOWS_UI_WEBUI_CORE_WEBUICOMMANDBARCONTRACT_VERSION)

#endif // defined(SPECIFIC_API_CONTRACT_DEFINITIONS)


// Header files for imported files
#include "inspectable.h"
#include "Windows.Foundation.h"
#include "Windows.UI.Xaml.Controls.h"
// Importing Collections header
#include <windows.foundation.collections.h>

#if defined(__cplusplus) && !defined(CINTERFACE)
#if defined(__MIDL_USE_C_ENUM)
#define MIDL_ENUM enum
#else
#define MIDL_ENUM enum class
#endif
/* Forward Declarations */
#ifndef ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IMethodDelegate;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIMethodDelegate ABI::react::uwp::IMethodDelegate

#endif // ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IMethodCallback;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIMethodCallback ABI::react::uwp::IMethodCallback

#endif // ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IMethodWithCallbackDelegate;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate ABI::react::uwp::IMethodWithCallbackDelegate

#endif // ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IModule;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIModule ABI::react::uwp::IModule

#endif // ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IInstance;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIInstance ABI::react::uwp::IInstance

#endif // ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IInstanceStatics;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIInstanceStatics ABI::react::uwp::IInstanceStatics

#endif // ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IReactControl;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIReactControl ABI::react::uwp::IReactControl

#endif // ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            interface IReactControlFactory;
        } /* react */
    } /* uwp */} /* ABI */
#define __x_ABI_Creact_Cuwp_CIReactControlFactory ABI::react::uwp::IReactControlFactory

#endif // ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__

// Parameterized interface forward declarations (C++)

// Collection interface definitions

#ifndef DEF___FIIterator_1_HSTRING_USE
#define DEF___FIIterator_1_HSTRING_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("8c304ebb-6615-50a4-8829-879ecd443236"))
IIterator<HSTRING> : IIterator_impl<HSTRING> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<String>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<HSTRING> __FIIterator_1_HSTRING_t;
#define __FIIterator_1_HSTRING ABI::Windows::Foundation::Collections::__FIIterator_1_HSTRING_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1_HSTRING ABI::Windows::Foundation::Collections::IIterator<HSTRING>
//#define __FIIterator_1_HSTRING_t ABI::Windows::Foundation::Collections::IIterator<HSTRING>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1_HSTRING_USE */




#ifndef DEF___FIIterable_1_HSTRING_USE
#define DEF___FIIterable_1_HSTRING_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("e2fcc7c1-3bfc-5a0b-b2b0-72e769d1cb7e"))
IIterable<HSTRING> : IIterable_impl<HSTRING> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<String>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<HSTRING> __FIIterable_1_HSTRING_t;
#define __FIIterable_1_HSTRING ABI::Windows::Foundation::Collections::__FIIterable_1_HSTRING_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1_HSTRING ABI::Windows::Foundation::Collections::IIterable<HSTRING>
//#define __FIIterable_1_HSTRING_t ABI::Windows::Foundation::Collections::IIterable<HSTRING>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1_HSTRING_USE */




#ifndef DEF___FIVectorView_1_HSTRING_USE
#define DEF___FIVectorView_1_HSTRING_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("2f13c006-a03a-5f69-b090-75a43e33423e"))
IVectorView<HSTRING> : IVectorView_impl<HSTRING> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IVectorView`1<String>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IVectorView<HSTRING> __FIVectorView_1_HSTRING_t;
#define __FIVectorView_1_HSTRING ABI::Windows::Foundation::Collections::__FIVectorView_1_HSTRING_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIVectorView_1_HSTRING ABI::Windows::Foundation::Collections::IVectorView<HSTRING>
//#define __FIVectorView_1_HSTRING_t ABI::Windows::Foundation::Collections::IVectorView<HSTRING>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIVectorView_1_HSTRING_USE */




#ifndef DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#define DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("3a97b495-1bb5-59bf-becf-12e0f77ee761"))
IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*> : IKeyValuePair_impl<HSTRING,ABI::react::uwp::IMethodDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IKeyValuePair`2<String, react.uwp.MethodDelegate>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*> __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t;
#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*>
//#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE */





#ifndef DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#define DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("56acab9b-9c13-54d7-ae75-58a536bda195"))
IIterator<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate*> : IIterator_impl<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Windows.Foundation.Collections.IKeyValuePair`2<String, react.uwp.MethodDelegate>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate*> __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t;
#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::__FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*>*>
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE */





#ifndef DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#define DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("72ecd191-05c6-5394-87e4-893b631d3498"))
IIterable<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate*> : IIterable_impl<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Windows.Foundation.Collections.IKeyValuePair`2<String, react.uwp.MethodDelegate>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate*> __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t;
#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::__FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*>*>
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_t ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodDelegate*>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_USE */




#ifndef DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#define DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("6a0961fd-7993-5650-8684-b7d95136ce4a"))
IMapView<HSTRING,ABI::react::uwp::IMethodDelegate*> : IMapView_impl<HSTRING,ABI::react::uwp::IMethodDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IMapView`2<String, react.uwp.MethodDelegate>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IMapView<HSTRING,ABI::react::uwp::IMethodDelegate*> __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_t;
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::__FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate ABI::Windows::Foundation::Collections::IMapView<HSTRING,ABI::react::uwp::IMethodDelegate*>
//#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_t ABI::Windows::Foundation::Collections::IMapView<HSTRING,ABI::react::uwp::IMethodDelegate*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_USE */




#ifndef DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#define DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("2efae327-b0ee-5823-803f-72264ca14473"))
IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*> : IKeyValuePair_impl<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IKeyValuePair`2<String, react.uwp.MethodWithCallbackDelegate>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*> __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t;
#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>
//#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE */





#ifndef DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#define DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("c6039ea7-16c8-51f1-b66c-8f6c044c94e2"))
IIterator<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate*> : IIterator_impl<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Windows.Foundation.Collections.IKeyValuePair`2<String, react.uwp.MethodWithCallbackDelegate>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate*> __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t;
#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::__FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>*>
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE */





#ifndef DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#define DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("4e7b2ea1-ec57-5704-944d-11ed6e031cbe"))
IIterable<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate*> : IIterable_impl<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Windows.Foundation.Collections.IKeyValuePair`2<String, react.uwp.MethodWithCallbackDelegate>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<__FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate*> __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t;
#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::__FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>*>
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE */




#ifndef DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#define DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("8aec3378-8967-5323-886d-32f5e3ed2626"))
IMapView<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*> : IMapView_impl<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IMapView`2<String, react.uwp.MethodWithCallbackDelegate>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IMapView<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*> __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t;
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::__FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate ABI::Windows::Foundation::Collections::IMapView<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>
//#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_t ABI::Windows::Foundation::Collections::IMapView<HSTRING,ABI::react::uwp::IMethodWithCallbackDelegate*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_USE */




#ifndef DEF___FIKeyValuePair_2_HSTRING_HSTRING_USE
#define DEF___FIKeyValuePair_2_HSTRING_HSTRING_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("60310303-49c5-52e6-abc6-a9b36eccc716"))
IKeyValuePair<HSTRING,HSTRING> : IKeyValuePair_impl<HSTRING,HSTRING> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IKeyValuePair`2<String, String>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IKeyValuePair<HSTRING,HSTRING> __FIKeyValuePair_2_HSTRING_HSTRING_t;
#define __FIKeyValuePair_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::__FIKeyValuePair_2_HSTRING_HSTRING_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIKeyValuePair_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,HSTRING>
//#define __FIKeyValuePair_2_HSTRING_HSTRING_t ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,HSTRING>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIKeyValuePair_2_HSTRING_HSTRING_USE */





#ifndef DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_USE
#define DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("05eb86f1-7140-5517-b88d-cbaebe57e6b1"))
IIterator<__FIKeyValuePair_2_HSTRING_HSTRING*> : IIterator_impl<__FIKeyValuePair_2_HSTRING_HSTRING*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterator`1<Windows.Foundation.Collections.IKeyValuePair`2<String, String>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterator<__FIKeyValuePair_2_HSTRING_HSTRING*> __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_t;
#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::__FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,HSTRING>*>
//#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_t ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,HSTRING>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_USE */





#ifndef DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_USE
#define DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("e9bdaaf0-cbf6-5c72-be90-29cbf3a1319b"))
IIterable<__FIKeyValuePair_2_HSTRING_HSTRING*> : IIterable_impl<__FIKeyValuePair_2_HSTRING_HSTRING*> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IIterable`1<Windows.Foundation.Collections.IKeyValuePair`2<String, String>>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IIterable<__FIKeyValuePair_2_HSTRING_HSTRING*> __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_t;
#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::__FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,HSTRING>*>
//#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_t ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,HSTRING>*>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_USE */




#ifndef DEF___FIMapView_2_HSTRING_HSTRING_USE
#define DEF___FIMapView_2_HSTRING_HSTRING_USE
#if !defined(RO_NO_TEMPLATE_NAME)
namespace ABI { namespace Windows { namespace Foundation { namespace Collections {
template <>
struct __declspec(uuid("ac7f26f2-feb7-5b2a-8ac4-345bc62caede"))
IMapView<HSTRING,HSTRING> : IMapView_impl<HSTRING,HSTRING> 
{
    static const wchar_t* z_get_rc_name_impl() 
    {
        return L"Windows.Foundation.Collections.IMapView`2<String, String>"; 
    }
};
// Define a typedef for the parameterized interface specialization's mangled name.
// This allows code which uses the mangled name for the parameterized interface to access the
// correct parameterized interface specialization.
typedef IMapView<HSTRING,HSTRING> __FIMapView_2_HSTRING_HSTRING_t;
#define __FIMapView_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::__FIMapView_2_HSTRING_HSTRING_t
/* ABI */ } /* Windows */ } /* Foundation */ } /* Collections */ }

////  Define an alias for the C version of the interface for compatibility purposes.
//#define __FIMapView_2_HSTRING_HSTRING ABI::Windows::Foundation::Collections::IMapView<HSTRING,HSTRING>
//#define __FIMapView_2_HSTRING_HSTRING_t ABI::Windows::Foundation::Collections::IMapView<HSTRING,HSTRING>
#endif // !defined(RO_NO_TEMPLATE_NAME)
#endif /* DEF___FIMapView_2_HSTRING_HSTRING_USE */





namespace ABI {
    namespace react {
        namespace uwp {
            class ReactControl;
        } /* react */
    } /* uwp */} /* ABI */




/*
 *
 * Delegate react.uwp.MethodDelegate
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("264B6078-B38B-4C86-A796-8D2B1BC5CBCA"), version] */
            MIDL_INTERFACE("264B6078-B38B-4C86-A796-8D2B1BC5CBCA")
            IMethodDelegate : public IUnknown
            {
            public:
                virtual HRESULT STDMETHODCALLTYPE Invoke(
                    /* [in] */HSTRING args
                    ) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IMethodDelegate=_uuidof(IMethodDelegate);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodDelegate;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__) */


/*
 *
 * Delegate react.uwp.MethodCallback
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("9E1F6C38-0270-4F22-9B90-7DA312DB60B9"), version] */
            MIDL_INTERFACE("9E1F6C38-0270-4F22-9B90-7DA312DB60B9")
            IMethodCallback : public IUnknown
            {
            public:
                virtual HRESULT STDMETHODCALLTYPE Invoke(
                    __FIVectorView_1_HSTRING * pCallBack
                    ) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IMethodCallback=_uuidof(IMethodCallback);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodCallback;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__) */


/*
 *
 * Delegate react.uwp.MethodWithCallbackDelegate
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("C9ED1358-8239-4E71-BBF1-776200E841B2"), version] */
            MIDL_INTERFACE("C9ED1358-8239-4E71-BBF1-776200E841B2")
            IMethodWithCallbackDelegate : public IUnknown
            {
            public:
                virtual HRESULT STDMETHODCALLTYPE Invoke(
                    /* [in] */HSTRING args,
                    /* [in] */ABI::react::uwp::IMethodCallback  * callBack
                    ) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IMethodWithCallbackDelegate=_uuidof(IMethodWithCallbackDelegate);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__) */



/*
 *
 * Interface react.uwp.IModule
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IModule[] = L"react.uwp.IModule";
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("765C0317-7210-4E72-873B-CF0384B2B606"), version] */
            MIDL_INTERFACE("765C0317-7210-4E72-873B-CF0384B2B606")
            IModule : public IInspectable
            {
            public:
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Name(
                    /* [retval, out] */HSTRING * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Constants(
                    /* [retval, out] */__FIMapView_2_HSTRING_HSTRING * * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Methods(
                    /* [retval, out] */__FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * * value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_MethodsWithCallback(
                    /* [retval, out] */__FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * * value
                    ) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IModule=_uuidof(IModule);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIModule;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__) */


/*
 *
 * Struct react.uwp.InstanceSettings
 *
 */

namespace ABI {
    namespace react {
        namespace uwp {
            /* [version] */
            struct InstanceSettings
            {
                boolean UseWebDebugger;
                boolean UseLiveReload;
            };
            
        } /* react */
    } /* uwp */} /* ABI */


/*
 *
 * Interface react.uwp.IInstance
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IInstance[] = L"react.uwp.IInstance";
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("4A7C4023-0A50-4599-97A6-095528319E71"), version] */
            MIDL_INTERFACE("4A7C4023-0A50-4599-97A6-095528319E71")
            IInstance : public IInspectable
            {
            public:
                virtual HRESULT STDMETHODCALLTYPE Start(
                    /* [in] */struct ABI::react::uwp::InstanceSettings settings
                    ) = 0;
                virtual HRESULT STDMETHODCALLTYPE RegisterModule(
                    /* [in] */ABI::react::uwp::IModule * pModule
                    ) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IInstance=_uuidof(IInstance);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIInstance;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__) */


/*
 *
 * Interface react.uwp.IInstanceStatics
 *
 * Interface is a part of the implementation of type react.uwp.Instance
 *
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IInstanceStatics[] = L"react.uwp.IInstanceStatics";
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("A0B3AE3E-3E34-436D-BE4F-603A2C7098DE"), version, exclusiveto] */
            MIDL_INTERFACE("A0B3AE3E-3E34-436D-BE4F-603A2C7098DE")
            IInstanceStatics : public IInspectable
            {
            public:
                virtual HRESULT STDMETHODCALLTYPE Create(
                    /* [in] */HSTRING bundleFileName,
                    /* [retval, out] */ABI::react::uwp::IInstance * * instance
                    ) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IInstanceStatics=_uuidof(IInstanceStatics);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIInstanceStatics;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__) */


/*
 *
 * Class react.uwp.Instance
 *
 * RuntimeClass contains static methods.
 *
 * Class implements the following interfaces:
 *    react.uwp.IInstance ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_react_uwp_Instance_DEFINED
#define RUNTIMECLASS_react_uwp_Instance_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_react_uwp_Instance[] = L"react.uwp.Instance";
#endif


/*
 *
 * Interface react.uwp.IReactControl
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IReactControl[] = L"react.uwp.IReactControl";
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("BA8CD9D5-78E7-451E-9FA6-FAEAE6B43C52"), version] */
            MIDL_INTERFACE("BA8CD9D5-78E7-451E-9FA6-FAEAE6B43C52")
            IReactControl : public IInspectable
            {
            public:
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_JsComponentName(
                    /* [retval, out] */HSTRING * value
                    ) = 0;
                /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_JsComponentName(
                    /* [in] */HSTRING value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_InitialProps(
                    /* [retval, out] */HSTRING * value
                    ) = 0;
                /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_InitialProps(
                    /* [in] */HSTRING value
                    ) = 0;
                /* [propget] */virtual HRESULT STDMETHODCALLTYPE get_Instance(
                    /* [retval, out] */ABI::react::uwp::IInstance * * value
                    ) = 0;
                /* [propput] */virtual HRESULT STDMETHODCALLTYPE put_Instance(
                    /* [in] */ABI::react::uwp::IInstance * value
                    ) = 0;
                virtual HRESULT STDMETHODCALLTYPE StartRender(void) = 0;
                virtual HRESULT STDMETHODCALLTYPE StopRender(void) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IReactControl=_uuidof(IReactControl);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIReactControl;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__) */


/*
 *
 * Interface react.uwp.IReactControlFactory
 *
 * Interface is a part of the implementation of type react.uwp.ReactControl
 *
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IReactControlFactory[] = L"react.uwp.IReactControlFactory";
namespace ABI {
    namespace react {
        namespace uwp {
            /* [object, uuid("B882C924-0A23-4B4A-9088-8FAECB2D5945"), version, exclusiveto] */
            MIDL_INTERFACE("B882C924-0A23-4B4A-9088-8FAECB2D5945")
            IReactControlFactory : public IInspectable
            {
            public:
                virtual HRESULT STDMETHODCALLTYPE CreateInstance(
                    /* [in] */IInspectable * outer,
                    /* [out] */IInspectable * * inner,
                    /* [retval, out] */ABI::react::uwp::IReactControl * * instance
                    ) = 0;
                
            };

            extern MIDL_CONST_ID IID & IID_IReactControlFactory=_uuidof(IReactControlFactory);
            
        } /* react */
    } /* uwp */} /* ABI */

EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIReactControlFactory;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__) */


/*
 *
 * Class react.uwp.ReactControl
 *
 * Class implements the following interfaces:
 *    react.uwp.IReactControl ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_react_uwp_ReactControl_DEFINED
#define RUNTIMECLASS_react_uwp_ReactControl_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_react_uwp_ReactControl[] = L"react.uwp.ReactControl";
#endif



#else // !defined(__cplusplus)
/* Forward Declarations */
#ifndef ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIMethodDelegate __x_ABI_Creact_Cuwp_CIMethodDelegate;

#endif // ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIMethodCallback __x_ABI_Creact_Cuwp_CIMethodCallback;

#endif // ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate;

#endif // ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIModule __x_ABI_Creact_Cuwp_CIModule;

#endif // ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIInstance __x_ABI_Creact_Cuwp_CIInstance;

#endif // ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIInstanceStatics __x_ABI_Creact_Cuwp_CIInstanceStatics;

#endif // ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIReactControl __x_ABI_Creact_Cuwp_CIReactControl;

#endif // ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__

#ifndef ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIReactControlFactory __x_ABI_Creact_Cuwp_CIReactControlFactory;

#endif // ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__

// Parameterized interface forward declarations (C)

// Collection interface definitions
#if !defined(____FIIterator_1_HSTRING_INTERFACE_DEFINED__)
#define ____FIIterator_1_HSTRING_INTERFACE_DEFINED__

typedef interface __FIIterator_1_HSTRING __FIIterator_1_HSTRING;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1_HSTRING;

typedef struct __FIIterator_1_HSTRINGVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1_HSTRING * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1_HSTRING * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1_HSTRING * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1_HSTRING * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1_HSTRING * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1_HSTRING * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1_HSTRING * This, /* [retval][out] */ __RPC__out HSTRING *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1_HSTRING * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1_HSTRING * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1_HSTRING * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) HSTRING *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1_HSTRINGVtbl;

interface __FIIterator_1_HSTRING
{
    CONST_VTBL struct __FIIterator_1_HSTRINGVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1_HSTRING_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_HSTRING_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_HSTRING_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_HSTRING_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_HSTRING_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_HSTRING_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_HSTRING_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_HSTRING_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_HSTRING_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_HSTRING_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1_HSTRING_INTERFACE_DEFINED__


#if !defined(____FIIterable_1_HSTRING_INTERFACE_DEFINED__)
#define ____FIIterable_1_HSTRING_INTERFACE_DEFINED__

typedef interface __FIIterable_1_HSTRING __FIIterable_1_HSTRING;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1_HSTRING;

typedef  struct __FIIterable_1_HSTRINGVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1_HSTRING * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1_HSTRING * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1_HSTRING * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1_HSTRING * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1_HSTRING * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1_HSTRING * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1_HSTRING * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1_HSTRING **first);

    END_INTERFACE
} __FIIterable_1_HSTRINGVtbl;

interface __FIIterable_1_HSTRING
{
    CONST_VTBL struct __FIIterable_1_HSTRINGVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1_HSTRING_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_HSTRING_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_HSTRING_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_HSTRING_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_HSTRING_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_HSTRING_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_HSTRING_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1_HSTRING_INTERFACE_DEFINED__


#if !defined(____FIVectorView_1_HSTRING_INTERFACE_DEFINED__)
#define ____FIVectorView_1_HSTRING_INTERFACE_DEFINED__

typedef interface __FIVectorView_1_HSTRING __FIVectorView_1_HSTRING;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIVectorView_1_HSTRING;

typedef struct __FIVectorView_1_HSTRINGVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIVectorView_1_HSTRING * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )( __RPC__in __FIVectorView_1_HSTRING * This);

    ULONG ( STDMETHODCALLTYPE *Release )( __RPC__in __FIVectorView_1_HSTRING * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )( __RPC__in __FIVectorView_1_HSTRING * This,
                                            /* [out] */ __RPC__out ULONG *iidCount,
                                            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
        __RPC__in __FIVectorView_1_HSTRING * This,
            /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
        __RPC__in __FIVectorView_1_HSTRING * This,
            /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *GetAt )( 
                                         __RPC__in __FIVectorView_1_HSTRING * This,
                                         /* [in] */ unsigned int index,
                                         /* [retval][out] */ __RPC__out HSTRING *item);

        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __RPC__in __FIVectorView_1_HSTRING * This,
            /* [retval][out] */ __RPC__out unsigned int *size);

        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
                                               __RPC__in __FIVectorView_1_HSTRING * This,
            /* [in] */ HSTRING item,
            /* [out] */ __RPC__out unsigned int *index,
            /* [retval][out] */ __RPC__out boolean *found);

        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
                                               __RPC__in __FIVectorView_1_HSTRING * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) HSTRING *items,
            /* [retval][out] */ __RPC__out unsigned int *actual);

        END_INTERFACE
} __FIVectorView_1_HSTRINGVtbl;

interface __FIVectorView_1_HSTRING
{
    CONST_VTBL struct __FIVectorView_1_HSTRINGVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIVectorView_1_HSTRING_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_HSTRING_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_HSTRING_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_HSTRING_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_HSTRING_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_HSTRING_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_HSTRING_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_HSTRING_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_HSTRING_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_HSTRING_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */



#endif // ____FIVectorView_1_HSTRING_INTERFACE_DEFINED__


#if !defined(____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__)
#define ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

typedef interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

typedef struct __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ __RPC__out ULONG *iidCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [retval][out] */ __RPC__out HSTRING *key);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [retval][out] */ __RPC__deref_out_opt __x_ABI_Creact_Cuwp_CIMethodDelegate * *value);
    END_INTERFACE
} __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl;

interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
{
    CONST_VTBL struct __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_get_Key(This,key)	\
    ( (This)->lpVtbl -> get_Key(This,key) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 
#endif /* COBJMACROS */


#endif // ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__



#if !defined(____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__)
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

typedef interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

typedef struct __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [retval][out] */ __RPC__out __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl;

interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
{
    CONST_VTBL struct __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__



#if !defined(____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__)
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

typedef interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

typedef  struct __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate **first);

    END_INTERFACE
} __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl;

interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
{
    CONST_VTBL struct __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__


#if !defined(____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__)
#define ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

typedef interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate;

typedef struct __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,/* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *Lookup )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
        /* [in] */ __RPC__in HSTRING key,
        /* [retval][out] */ __RPC__deref_out_opt __x_ABI_Creact_Cuwp_CIMethodDelegate * *value);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [retval][out] */ __RPC__out unsigned int *size);
    HRESULT ( STDMETHODCALLTYPE *HasKey )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This, /* [in] */ __RPC__in HSTRING key, /* [retval][out] */ __RPC__out boolean *found);
    HRESULT ( STDMETHODCALLTYPE *Split )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,/* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **firstPartition,
        /* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **secondPartition);
    END_INTERFACE
} __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl;

interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate
{
    CONST_VTBL struct __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_Lookup(This,key,value)	\
    ( (This)->lpVtbl -> Lookup(This,key,value) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_HasKey(This,key,found)	\
    ( (This)->lpVtbl -> HasKey(This,key,found) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_Split(This,firstPartition,secondPartition)	\
    ( (This)->lpVtbl -> Split(This,firstPartition,secondPartition) ) 
#endif /* COBJMACROS */


#endif // ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__


#if !defined(____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__)
#define ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

typedef interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

typedef struct __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ __RPC__out ULONG *iidCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [retval][out] */ __RPC__out HSTRING *key);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )(__RPC__in __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [retval][out] */ __RPC__deref_out_opt __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * *value);
    END_INTERFACE
} __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl;

interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
{
    CONST_VTBL struct __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_get_Key(This,key)	\
    ( (This)->lpVtbl -> get_Key(This,key) ) 

#define __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 
#endif /* COBJMACROS */


#endif // ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__



#if !defined(____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__)
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

typedef interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

typedef struct __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [retval][out] */ __RPC__out __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl;

interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
{
    CONST_VTBL struct __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__



#if !defined(____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__)
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

typedef interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

typedef  struct __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **first);

    END_INTERFACE
} __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl;

interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
{
    CONST_VTBL struct __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__


#if !defined(____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__)
#define ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

typedef interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

typedef struct __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,/* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *Lookup )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
        /* [in] */ __RPC__in HSTRING key,
        /* [retval][out] */ __RPC__deref_out_opt __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * *value);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [retval][out] */ __RPC__out unsigned int *size);
    HRESULT ( STDMETHODCALLTYPE *HasKey )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This, /* [in] */ __RPC__in HSTRING key, /* [retval][out] */ __RPC__out boolean *found);
    HRESULT ( STDMETHODCALLTYPE *Split )(__RPC__in __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,/* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **firstPartition,
        /* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **secondPartition);
    END_INTERFACE
} __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl;

interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
{
    CONST_VTBL struct __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_Lookup(This,key,value)	\
    ( (This)->lpVtbl -> Lookup(This,key,value) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_HasKey(This,key,found)	\
    ( (This)->lpVtbl -> HasKey(This,key,found) ) 
#define __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_Split(This,firstPartition,secondPartition)	\
    ( (This)->lpVtbl -> Split(This,firstPartition,secondPartition) ) 
#endif /* COBJMACROS */


#endif // ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__


#if !defined(____FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__)
#define ____FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__

typedef interface __FIKeyValuePair_2_HSTRING_HSTRING __FIKeyValuePair_2_HSTRING_HSTRING;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIKeyValuePair_2_HSTRING_HSTRING;

typedef struct __FIKeyValuePair_2_HSTRING_HSTRINGVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This,
            /* [out] */ __RPC__out ULONG *iidCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);
    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This, /* [retval][out] */ __RPC__out HSTRING *key);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )(__RPC__in __FIKeyValuePair_2_HSTRING_HSTRING * This, /* [retval][out] */ __RPC__deref_out_opt HSTRING *value);
    END_INTERFACE
} __FIKeyValuePair_2_HSTRING_HSTRINGVtbl;

interface __FIKeyValuePair_2_HSTRING_HSTRING
{
    CONST_VTBL struct __FIKeyValuePair_2_HSTRING_HSTRINGVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIKeyValuePair_2_HSTRING_HSTRING_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIKeyValuePair_2_HSTRING_HSTRING_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIKeyValuePair_2_HSTRING_HSTRING_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIKeyValuePair_2_HSTRING_HSTRING_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIKeyValuePair_2_HSTRING_HSTRING_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIKeyValuePair_2_HSTRING_HSTRING_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIKeyValuePair_2_HSTRING_HSTRING_get_Key(This,key)	\
    ( (This)->lpVtbl -> get_Key(This,key) ) 

#define __FIKeyValuePair_2_HSTRING_HSTRING_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 
#endif /* COBJMACROS */


#endif // ____FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__



#if !defined(____FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__)
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__

typedef interface __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING;

typedef struct __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);
    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This);
    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This);
    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [retval][out] */ __RPC__out __FIKeyValuePair_2_HSTRING_HSTRING * *current);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *MoveNext )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [retval][out] */ __RPC__out boolean *hasCurrent);
    HRESULT ( STDMETHODCALLTYPE *GetMany )(__RPC__in __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING * This,
        /* [in] */ unsigned int capacity,
        /* [size_is][length_is][out] */ __RPC__out_ecount_part(capacity, *actual) __FIKeyValuePair_2_HSTRING_HSTRING * *items,
        /* [retval][out] */ __RPC__out unsigned int *actual);

    END_INTERFACE
} __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl;

interface __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING
{
    CONST_VTBL struct __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif // ____FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__



#if !defined(____FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__)
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__

typedef interface __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING;

typedef  struct __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl
{
    BEGIN_INTERFACE

    HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
        __RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING * This,
                                           /* [out] */ __RPC__out ULONG *iidCount,
                                           /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);

    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *First )(__RPC__in __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING * This, /* [retval][out] */ __RPC__deref_out_opt __FIIterator_1___FIKeyValuePair_2_HSTRING_HSTRING **first);

    END_INTERFACE
} __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl;

interface __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING
{
    CONST_VTBL struct __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRINGVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif // ____FIIterable_1___FIKeyValuePair_2_HSTRING_HSTRING_INTERFACE_DEFINED__


#if !defined(____FIMapView_2_HSTRING_HSTRING_INTERFACE_DEFINED__)
#define ____FIMapView_2_HSTRING_HSTRING_INTERFACE_DEFINED__

typedef interface __FIMapView_2_HSTRING_HSTRING __FIMapView_2_HSTRING_HSTRING;

//  Declare the parameterized interface IID.
EXTERN_C const IID IID___FIMapView_2_HSTRING_HSTRING;

typedef struct __FIMapView_2_HSTRING_HSTRINGVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This,
        /* [in] */ __RPC__in REFIID riid,
        /* [annotation][iid_is][out] */ 
        _COM_Outptr_  void **ppvObject);

    ULONG ( STDMETHODCALLTYPE *AddRef )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This);

    ULONG ( STDMETHODCALLTYPE *Release )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This);

    HRESULT ( STDMETHODCALLTYPE *GetIids )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This,
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids);

    HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This, /* [out] */ __RPC__deref_out_opt HSTRING *className);
    HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This,/* [out] */ __RPC__out TrustLevel *trustLevel);

    HRESULT ( STDMETHODCALLTYPE *Lookup )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This,
        /* [in] */ __RPC__in HSTRING key,
        /* [retval][out] */ __RPC__deref_out_opt HSTRING *value);
    /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This, /* [retval][out] */ __RPC__out unsigned int *size);
    HRESULT ( STDMETHODCALLTYPE *HasKey )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This, /* [in] */ __RPC__in HSTRING key, /* [retval][out] */ __RPC__out boolean *found);
    HRESULT ( STDMETHODCALLTYPE *Split )(__RPC__in __FIMapView_2_HSTRING_HSTRING * This,/* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_HSTRING **firstPartition,
        /* [out] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_HSTRING **secondPartition);
    END_INTERFACE
} __FIMapView_2_HSTRING_HSTRINGVtbl;

interface __FIMapView_2_HSTRING_HSTRING
{
    CONST_VTBL struct __FIMapView_2_HSTRING_HSTRINGVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __FIMapView_2_HSTRING_HSTRING_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define __FIMapView_2_HSTRING_HSTRING_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define __FIMapView_2_HSTRING_HSTRING_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 

#define __FIMapView_2_HSTRING_HSTRING_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 
#define __FIMapView_2_HSTRING_HSTRING_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 
#define __FIMapView_2_HSTRING_HSTRING_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 

#define __FIMapView_2_HSTRING_HSTRING_Lookup(This,key,value)	\
    ( (This)->lpVtbl -> Lookup(This,key,value) ) 
#define __FIMapView_2_HSTRING_HSTRING_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 
#define __FIMapView_2_HSTRING_HSTRING_HasKey(This,key,found)	\
    ( (This)->lpVtbl -> HasKey(This,key,found) ) 
#define __FIMapView_2_HSTRING_HSTRING_Split(This,firstPartition,secondPartition)	\
    ( (This)->lpVtbl -> Split(This,firstPartition,secondPartition) ) 
#endif /* COBJMACROS */


#endif // ____FIMapView_2_HSTRING_HSTRING_INTERFACE_DEFINED__






/*
 *
 * Delegate react.uwp.MethodDelegate
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__
/* [object, uuid("264B6078-B38B-4C86-A796-8D2B1BC5CBCA"), version] */
typedef struct __x_ABI_Creact_Cuwp_CIMethodDelegateVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodDelegate * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodDelegate * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodDelegate * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_Creact_Cuwp_CIMethodDelegate * This,
        /* [in] */HSTRING args
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIMethodDelegateVtbl;

interface __x_ABI_Creact_Cuwp_CIMethodDelegate
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIMethodDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIMethodDelegate_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIMethodDelegate_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIMethodDelegate_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIMethodDelegate_Invoke(This,args) \
    ( (This)->lpVtbl->Invoke(This,args) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodDelegate;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__) */


/*
 *
 * Delegate react.uwp.MethodCallback
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__
/* [object, uuid("9E1F6C38-0270-4F22-9B90-7DA312DB60B9"), version] */
typedef struct __x_ABI_Creact_Cuwp_CIMethodCallbackVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodCallback * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodCallback * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodCallback * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_Creact_Cuwp_CIMethodCallback * This,
        __FIVectorView_1_HSTRING * pCallBack
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIMethodCallbackVtbl;

interface __x_ABI_Creact_Cuwp_CIMethodCallback
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIMethodCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIMethodCallback_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIMethodCallback_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIMethodCallback_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIMethodCallback_Invoke(This,pCallBack) \
    ( (This)->lpVtbl->Invoke(This,pCallBack) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodCallback;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__) */


/*
 *
 * Delegate react.uwp.MethodWithCallbackDelegate
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__
/* [object, uuid("C9ED1358-8239-4E71-BBF1-776200E841B2"), version] */
typedef struct __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegateVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This);

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This);
HRESULT ( STDMETHODCALLTYPE *Invoke )(
        __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This,
        /* [in] */HSTRING args,
        /* [in] */__x_ABI_Creact_Cuwp_CIMethodCallback  * callBack
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegateVtbl;

interface __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_Invoke(This,args,callBack) \
    ( (This)->lpVtbl->Invoke(This,args,callBack) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__) */



/*
 *
 * Interface react.uwp.IModule
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IModule[] = L"react.uwp.IModule";
/* [object, uuid("765C0317-7210-4E72-873B-CF0384B2B606"), version] */
typedef struct __x_ABI_Creact_Cuwp_CIModuleVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_Creact_Cuwp_CIModule * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIModule * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIModule * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_Creact_Cuwp_CIModule * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_Creact_Cuwp_CIModule * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_Creact_Cuwp_CIModule * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Name )(
        __x_ABI_Creact_Cuwp_CIModule * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Constants )(
        __x_ABI_Creact_Cuwp_CIModule * This,
        /* [retval, out] */__FIMapView_2_HSTRING_HSTRING * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Methods )(
        __x_ABI_Creact_Cuwp_CIModule * This,
        /* [retval, out] */__FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * * value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_MethodsWithCallback )(
        __x_ABI_Creact_Cuwp_CIModule * This,
        /* [retval, out] */__FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * * value
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIModuleVtbl;

interface __x_ABI_Creact_Cuwp_CIModule
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIModuleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIModule_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIModule_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIModule_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIModule_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_Creact_Cuwp_CIModule_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_Creact_Cuwp_CIModule_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_Creact_Cuwp_CIModule_get_Name(This,value) \
    ( (This)->lpVtbl->get_Name(This,value) )

#define __x_ABI_Creact_Cuwp_CIModule_get_Constants(This,value) \
    ( (This)->lpVtbl->get_Constants(This,value) )

#define __x_ABI_Creact_Cuwp_CIModule_get_Methods(This,value) \
    ( (This)->lpVtbl->get_Methods(This,value) )

#define __x_ABI_Creact_Cuwp_CIModule_get_MethodsWithCallback(This,value) \
    ( (This)->lpVtbl->get_MethodsWithCallback(This,value) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIModule;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__) */


/*
 *
 * Struct react.uwp.InstanceSettings
 *
 */

/* [version] */
struct __x_ABI_Creact_Cuwp_CInstanceSettings
{
    boolean UseWebDebugger;
    boolean UseLiveReload;
};


/*
 *
 * Interface react.uwp.IInstance
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IInstance[] = L"react.uwp.IInstance";
/* [object, uuid("4A7C4023-0A50-4599-97A6-095528319E71"), version] */
typedef struct __x_ABI_Creact_Cuwp_CIInstanceVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstance * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstance * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstance * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstance * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstance * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstance * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Start )(
        __x_ABI_Creact_Cuwp_CIInstance * This,
        /* [in] */struct __x_ABI_Creact_Cuwp_CInstanceSettings settings
        );
    HRESULT ( STDMETHODCALLTYPE *RegisterModule )(
        __x_ABI_Creact_Cuwp_CIInstance * This,
        /* [in] */__x_ABI_Creact_Cuwp_CIModule * pModule
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIInstanceVtbl;

interface __x_ABI_Creact_Cuwp_CIInstance
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIInstanceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIInstance_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIInstance_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIInstance_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIInstance_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_Creact_Cuwp_CIInstance_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_Creact_Cuwp_CIInstance_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_Creact_Cuwp_CIInstance_Start(This,settings) \
    ( (This)->lpVtbl->Start(This,settings) )

#define __x_ABI_Creact_Cuwp_CIInstance_RegisterModule(This,pModule) \
    ( (This)->lpVtbl->RegisterModule(This,pModule) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIInstance;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__) */


/*
 *
 * Interface react.uwp.IInstanceStatics
 *
 * Interface is a part of the implementation of type react.uwp.Instance
 *
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IInstanceStatics[] = L"react.uwp.IInstanceStatics";
/* [object, uuid("A0B3AE3E-3E34-436D-BE4F-603A2C7098DE"), version, exclusiveto] */
typedef struct __x_ABI_Creact_Cuwp_CIInstanceStaticsVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstanceStatics * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstanceStatics * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *Create )(
        __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
        /* [in] */HSTRING bundleFileName,
        /* [retval, out] */__x_ABI_Creact_Cuwp_CIInstance * * instance
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIInstanceStaticsVtbl;

interface __x_ABI_Creact_Cuwp_CIInstanceStatics
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIInstanceStaticsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIInstanceStatics_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_Create(This,bundleFileName,instance) \
    ( (This)->lpVtbl->Create(This,bundleFileName,instance) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIInstanceStatics;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__) */


/*
 *
 * Class react.uwp.Instance
 *
 * RuntimeClass contains static methods.
 *
 * Class implements the following interfaces:
 *    react.uwp.IInstance ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_react_uwp_Instance_DEFINED
#define RUNTIMECLASS_react_uwp_Instance_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_react_uwp_Instance[] = L"react.uwp.Instance";
#endif


/*
 *
 * Interface react.uwp.IReactControl
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IReactControl[] = L"react.uwp.IReactControl";
/* [object, uuid("BA8CD9D5-78E7-451E-9FA6-FAEAE6B43C52"), version] */
typedef struct __x_ABI_Creact_Cuwp_CIReactControlVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControl * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControl * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControl * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControl * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControl * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControl * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
/* [propget] */HRESULT ( STDMETHODCALLTYPE *get_JsComponentName )(
        __x_ABI_Creact_Cuwp_CIReactControl * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_JsComponentName )(
        __x_ABI_Creact_Cuwp_CIReactControl * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_InitialProps )(
        __x_ABI_Creact_Cuwp_CIReactControl * This,
        /* [retval, out] */HSTRING * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_InitialProps )(
        __x_ABI_Creact_Cuwp_CIReactControl * This,
        /* [in] */HSTRING value
        );
    /* [propget] */HRESULT ( STDMETHODCALLTYPE *get_Instance )(
        __x_ABI_Creact_Cuwp_CIReactControl * This,
        /* [retval, out] */__x_ABI_Creact_Cuwp_CIInstance * * value
        );
    /* [propput] */HRESULT ( STDMETHODCALLTYPE *put_Instance )(
        __x_ABI_Creact_Cuwp_CIReactControl * This,
        /* [in] */__x_ABI_Creact_Cuwp_CIInstance * value
        );
    HRESULT ( STDMETHODCALLTYPE *StartRender )(
        __x_ABI_Creact_Cuwp_CIReactControl * This
        );
    HRESULT ( STDMETHODCALLTYPE *StopRender )(
        __x_ABI_Creact_Cuwp_CIReactControl * This
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIReactControlVtbl;

interface __x_ABI_Creact_Cuwp_CIReactControl
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIReactControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIReactControl_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIReactControl_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIReactControl_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIReactControl_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_Creact_Cuwp_CIReactControl_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_Creact_Cuwp_CIReactControl_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_Creact_Cuwp_CIReactControl_get_JsComponentName(This,value) \
    ( (This)->lpVtbl->get_JsComponentName(This,value) )

#define __x_ABI_Creact_Cuwp_CIReactControl_put_JsComponentName(This,value) \
    ( (This)->lpVtbl->put_JsComponentName(This,value) )

#define __x_ABI_Creact_Cuwp_CIReactControl_get_InitialProps(This,value) \
    ( (This)->lpVtbl->get_InitialProps(This,value) )

#define __x_ABI_Creact_Cuwp_CIReactControl_put_InitialProps(This,value) \
    ( (This)->lpVtbl->put_InitialProps(This,value) )

#define __x_ABI_Creact_Cuwp_CIReactControl_get_Instance(This,value) \
    ( (This)->lpVtbl->get_Instance(This,value) )

#define __x_ABI_Creact_Cuwp_CIReactControl_put_Instance(This,value) \
    ( (This)->lpVtbl->put_Instance(This,value) )

#define __x_ABI_Creact_Cuwp_CIReactControl_StartRender(This) \
    ( (This)->lpVtbl->StartRender(This) )

#define __x_ABI_Creact_Cuwp_CIReactControl_StopRender(This) \
    ( (This)->lpVtbl->StopRender(This) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIReactControl;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__) */


/*
 *
 * Interface react.uwp.IReactControlFactory
 *
 * Interface is a part of the implementation of type react.uwp.ReactControl
 *
 *
 */
#if !defined(____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__)
#define ____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__
extern const __declspec(selectany) _Null_terminated_ WCHAR InterfaceName_react_uwp_IReactControlFactory[] = L"react.uwp.IReactControlFactory";
/* [object, uuid("B882C924-0A23-4B4A-9088-8FAECB2D5945"), version, exclusiveto] */
typedef struct __x_ABI_Creact_Cuwp_CIReactControlFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT ( STDMETHODCALLTYPE *QueryInterface)(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
    /* [in] */ __RPC__in REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject
    );

ULONG ( STDMETHODCALLTYPE *AddRef )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControlFactory * This
    );

ULONG ( STDMETHODCALLTYPE *Release )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControlFactory * This
    );

HRESULT ( STDMETHODCALLTYPE *GetIids )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
    /* [out] */ __RPC__out ULONG *iidCount,
    /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids
    );

HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
    /* [out] */ __RPC__deref_out_opt HSTRING *className
    );

HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )(
    __RPC__in __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
    /* [OUT ] */ __RPC__out TrustLevel *trustLevel
    );
HRESULT ( STDMETHODCALLTYPE *CreateInstance )(
        __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
        /* [in] */IInspectable * outer,
        /* [out] */IInspectable * * inner,
        /* [retval, out] */__x_ABI_Creact_Cuwp_CIReactControl * * instance
        );
    END_INTERFACE
    
} __x_ABI_Creact_Cuwp_CIReactControlFactoryVtbl;

interface __x_ABI_Creact_Cuwp_CIReactControlFactory
{
    CONST_VTBL struct __x_ABI_Creact_Cuwp_CIReactControlFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define __x_ABI_Creact_Cuwp_CIReactControlFactory_QueryInterface(This,riid,ppvObject) \
( (This)->lpVtbl->QueryInterface(This,riid,ppvObject) )

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_AddRef(This) \
        ( (This)->lpVtbl->AddRef(This) )

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_Release(This) \
        ( (This)->lpVtbl->Release(This) )

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_GetIids(This,iidCount,iids) \
        ( (This)->lpVtbl->GetIids(This,iidCount,iids) )

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_GetRuntimeClassName(This,className) \
        ( (This)->lpVtbl->GetRuntimeClassName(This,className) )

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_GetTrustLevel(This,trustLevel) \
        ( (This)->lpVtbl->GetTrustLevel(This,trustLevel) )

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_CreateInstance(This,outer,inner,instance) \
    ( (This)->lpVtbl->CreateInstance(This,outer,inner,instance) )


#endif /* COBJMACROS */


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIReactControlFactory;
#endif /* !defined(____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__) */


/*
 *
 * Class react.uwp.ReactControl
 *
 * Class implements the following interfaces:
 *    react.uwp.IReactControl ** Default Interface **
 *
 * Class Threading Model:  Both Single and Multi Threaded Apartment
 *
 * Class Marshaling Behavior:  Agile - Class is agile
 *
 */

#ifndef RUNTIMECLASS_react_uwp_ReactControl_DEFINED
#define RUNTIMECLASS_react_uwp_ReactControl_DEFINED
extern const __declspec(selectany) _Null_terminated_ WCHAR RuntimeClass_react_uwp_ReactControl[] = L"react.uwp.ReactControl";
#endif



#endif // defined(__cplusplus)
#pragma pop_macro("MIDL_CONST_ID")
#endif // __ReactControl2Eg_p_h__

#endif // __ReactControl2Eg_h__
