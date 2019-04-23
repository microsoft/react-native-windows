

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for C:\Users\kmelmon\AppData\Local\Temp\ReactControl.g.idl-a3898511:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ReactControl2Eg_p_h__
#define __ReactControl2Eg_p_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(__cplusplus)
#if defined(__MIDL_USE_C_ENUM)
#define MIDL_ENUM enum
#else
#define MIDL_ENUM enum class
#endif
#endif


/* Forward Declarations */ 

#ifndef ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIMethodDelegate __x_ABI_Creact_Cuwp_CIMethodDelegate;

#endif 	/* ____x_ABI_Creact_Cuwp_CIMethodDelegate_FWD_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIMethodCallback __x_ABI_Creact_Cuwp_CIMethodCallback;

#endif 	/* ____x_ABI_Creact_Cuwp_CIMethodCallback_FWD_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate;

#endif 	/* ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_FWD_DEFINED__ */


#ifndef ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
#define ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
typedef interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

#endif 	/* ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__ */


#ifndef ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
typedef interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

#endif 	/* ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__ */


#ifndef ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
typedef interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

#endif 	/* ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__ */


#ifndef ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
#define ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__
typedef interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate;

#endif 	/* ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_FWD_DEFINED__ */


#ifndef ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
#define ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
typedef interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#endif 	/* ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__ */


#ifndef ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
typedef interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#endif 	/* ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__ */


#ifndef ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
typedef interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#endif 	/* ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__ */


#ifndef ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
#define ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__
typedef interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#endif 	/* ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_FWD_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIModule __x_ABI_Creact_Cuwp_CIModule;

#endif 	/* ____x_ABI_Creact_Cuwp_CIModule_FWD_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIInstance __x_ABI_Creact_Cuwp_CIInstance;

#endif 	/* ____x_ABI_Creact_Cuwp_CIInstance_FWD_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIInstanceStatics __x_ABI_Creact_Cuwp_CIInstanceStatics;

#endif 	/* ____x_ABI_Creact_Cuwp_CIInstanceStatics_FWD_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIReactControl __x_ABI_Creact_Cuwp_CIReactControl;

#endif 	/* ____x_ABI_Creact_Cuwp_CIReactControl_FWD_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__
typedef interface __x_ABI_Creact_Cuwp_CIReactControlFactory __x_ABI_Creact_Cuwp_CIReactControlFactory;

#endif 	/* ____x_ABI_Creact_Cuwp_CIReactControlFactory_FWD_DEFINED__ */


/* header files for imported files */
#include "inspectable.h"
#include "Windows.Foundation.h"
#include "Windows.UI.Xaml.Controls.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_ReactControl2Eg_0000_0000 */
/* [local] */ 



























extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0000_v0_0_s_ifspec;

#ifndef ____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIMethodDelegate */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("264B6078-B38B-4C86-A796-8D2B1BC5CBCA")
    __x_ABI_Creact_Cuwp_CIMethodDelegate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ HSTRING args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIMethodDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIMethodDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIMethodDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIMethodDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            __x_ABI_Creact_Cuwp_CIMethodDelegate * This,
            /* [in] */ HSTRING args);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIMethodDelegateVtbl;

    interface __x_ABI_Creact_Cuwp_CIMethodDelegate
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIMethodDelegateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIMethodDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIMethodDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIMethodDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIMethodDelegate_Invoke(This,args)	\
    ( (This)->lpVtbl -> Invoke(This,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIMethodDelegate_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIMethodCallback */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9E1F6C38-0270-4F22-9B90-7DA312DB60B9")
    __x_ABI_Creact_Cuwp_CIMethodCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            __FIVectorView_1_HSTRING *pCallBack) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIMethodCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIMethodCallback * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIMethodCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIMethodCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            __x_ABI_Creact_Cuwp_CIMethodCallback * This,
            __FIVectorView_1_HSTRING *pCallBack);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIMethodCallbackVtbl;

    interface __x_ABI_Creact_Cuwp_CIMethodCallback
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIMethodCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIMethodCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIMethodCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIMethodCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIMethodCallback_Invoke(This,pCallBack)	\
    ( (This)->lpVtbl -> Invoke(This,pCallBack) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIMethodCallback_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C9ED1358-8239-4E71-BBF1-776200E841B2")
    __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ HSTRING args,
            /* [in] */ __x_ABI_Creact_Cuwp_CIMethodCallback *callBack) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate * This,
            /* [in] */ HSTRING args,
            /* [in] */ __x_ABI_Creact_Cuwp_CIMethodCallback *callBack);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegateVtbl;

    interface __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_Invoke(This,args,callBack)	\
    ( (This)->lpVtbl -> Invoke(This,args,callBack) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2638 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2638_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2638_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0004 */
/* [local] */ 

#ifndef DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
#define DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0004_v0_0_s_ifspec;

#ifndef ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__
#define ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

/* interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3a97b495-1bb5-59bf-becf-12e0f77ee761")
    __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Key( 
            /* [retval][out] */ HSTRING *key) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodDelegate **value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [retval][out] */ HSTRING *key);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodDelegate **value);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0005 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0005_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2639 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2639_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2639_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0006 */
/* [local] */ 

#ifndef DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
#define DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0006_v0_0_s_ifspec;

#ifndef ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

/* interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56acab9b-9c13-54d7-ae75-58a536bda195")
    __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate **current) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HasCurrent( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveNext( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [retval][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate **current);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *MoveNext )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate **items,
            /* [retval][out] */ unsigned int *actual);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0007 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0007_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2640 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2640_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2640_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0008 */
/* [local] */ 

#ifndef DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
#define DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0008_v0_0_s_ifspec;

#ifndef ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

/* interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72ecd191-05c6-5394-87e4-893b631d3498")
    __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate **first) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *First )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [retval][out] */ __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate **first);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0009 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0009_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2641 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2641_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2641_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0010 */
/* [local] */ 

#ifndef DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate
#define DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0010_v0_0_s_ifspec;

#ifndef ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__
#define ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__

/* interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6a0961fd-7993-5650-8684-b7d95136ce4a")
    __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Lookup( 
            /* [in] */ HSTRING key,
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodDelegate **value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ unsigned int *size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HasKey( 
            /* [in] */ HSTRING key,
            /* [retval][out] */ boolean *found) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Split( 
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **firstPartition,
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **secondPartition) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Lookup )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [in] */ HSTRING key,
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodDelegate **value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [retval][out] */ unsigned int *size);
        
        HRESULT ( STDMETHODCALLTYPE *HasKey )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [in] */ HSTRING key,
            /* [retval][out] */ boolean *found);
        
        HRESULT ( STDMETHODCALLTYPE *Split )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate * This,
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **firstPartition,
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **secondPartition);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0011 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0011_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2642 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2642_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2642_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0012 */
/* [local] */ 

#ifndef DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#define DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0012_v0_0_s_ifspec;

#ifndef ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__
#define ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

/* interface __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2efae327-b0ee-5823-803f-72264ca14473")
    __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Key( 
            /* [retval][out] */ HSTRING *key) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate **value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Key )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [retval][out] */ HSTRING *key);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate **value);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0013 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0013_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0013_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2643 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2643_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2643_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0014 */
/* [local] */ 

#ifndef DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#define DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0014_v0_0_s_ifspec;

#ifndef ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__
#define ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

/* interface __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c6039ea7-16c8-51f1-b66c-8f6c044c94e2")
    __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **current) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HasCurrent( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveNext( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [retval][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **current);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *MoveNext )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **items,
            /* [retval][out] */ unsigned int *actual);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0015 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0015_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0015_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2644 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2644_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2644_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0016 */
/* [local] */ 

#ifndef DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#define DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0016_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0016_v0_0_s_ifspec;

#ifndef ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__
#define ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

/* interface __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4e7b2ea1-ec57-5704-944d-11ed6e031cbe")
    __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **first) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *First )( 
            __FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [retval][out] */ __FIIterator_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **first);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0017 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterable_1___FIKeyValuePair_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0017_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0017_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg2Eidl_0000_2645 */




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2645_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg2Eidl_0000_2645_v0_0_s_ifspec;

/* interface __MIDL_itf_ReactControl2Eg_0000_0018 */
/* [local] */ 

#ifndef DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#define DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0018_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0018_v0_0_s_ifspec;

#ifndef ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__
#define ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__

/* interface __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8aec3378-8967-5323-886d-32f5e3ed2626")
    __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Lookup( 
            /* [in] */ HSTRING key,
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate **value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ unsigned int *size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HasKey( 
            /* [in] */ HSTRING key,
            /* [retval][out] */ boolean *found) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Split( 
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **firstPartition,
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **secondPartition) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Lookup )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [in] */ HSTRING key,
            /* [retval][out] */ __x_ABI_Creact_Cuwp_CIMethodWithCallbackDelegate **value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [retval][out] */ unsigned int *size);
        
        HRESULT ( STDMETHODCALLTYPE *HasKey )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [in] */ HSTRING key,
            /* [retval][out] */ boolean *found);
        
        HRESULT ( STDMETHODCALLTYPE *Split )( 
            __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate * This,
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **firstPartition,
            /* [out] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **secondPartition);
        
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


#endif 	/* C style interface */




#endif 	/* ____FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0019 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate */


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0019_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0019_v0_0_s_ifspec;

#ifndef ____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIModule */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIModule;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("765C0317-7210-4E72-873B-CF0384B2B606")
    __x_ABI_Creact_Cuwp_CIModule : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [out][retval] */ HSTRING *value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Constants( 
            /* [out][retval] */ __FIMapView_2_HSTRING_HSTRING **value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Methods( 
            /* [out][retval] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_MethodsWithCallback( 
            /* [out][retval] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIModuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIModule * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIModule * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [out][retval] */ HSTRING *value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Constants )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [out][retval] */ __FIMapView_2_HSTRING_HSTRING **value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Methods )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [out][retval] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodDelegate **value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_MethodsWithCallback )( 
            __x_ABI_Creact_Cuwp_CIModule * This,
            /* [out][retval] */ __FIMapView_2_HSTRING_react__Cuwp__CIMethodWithCallbackDelegate **value);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIModuleVtbl;

    interface __x_ABI_Creact_Cuwp_CIModule
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIModuleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIModule_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIModule_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIModule_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIModule_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_Creact_Cuwp_CIModule_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_Creact_Cuwp_CIModule_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_Creact_Cuwp_CIModule_get_Name(This,value)	\
    ( (This)->lpVtbl -> get_Name(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIModule_get_Constants(This,value)	\
    ( (This)->lpVtbl -> get_Constants(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIModule_get_Methods(This,value)	\
    ( (This)->lpVtbl -> get_Methods(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIModule_get_MethodsWithCallback(This,value)	\
    ( (This)->lpVtbl -> get_MethodsWithCallback(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIModule_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0020 */
/* [local] */ 

struct __x_ABI_Creact_Cuwp_CInstanceSettings
    {
    boolean UseWebDebugger;
    boolean UseLiveReload;
    } ;


extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0020_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0020_v0_0_s_ifspec;

#ifndef ____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIInstance */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIInstance;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4A7C4023-0A50-4599-97A6-095528319E71")
    __x_ABI_Creact_Cuwp_CIInstance : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Start( 
            /* [in] */ struct __x_ABI_Creact_Cuwp_CInstanceSettings settings) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterModule( 
            /* [in] */ __x_ABI_Creact_Cuwp_CIModule *pModule) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIInstanceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIInstance * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIInstance * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIInstance * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_Creact_Cuwp_CIInstance * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_Creact_Cuwp_CIInstance * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_Creact_Cuwp_CIInstance * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            __x_ABI_Creact_Cuwp_CIInstance * This,
            /* [in] */ struct __x_ABI_Creact_Cuwp_CInstanceSettings settings);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterModule )( 
            __x_ABI_Creact_Cuwp_CIInstance * This,
            /* [in] */ __x_ABI_Creact_Cuwp_CIModule *pModule);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIInstanceVtbl;

    interface __x_ABI_Creact_Cuwp_CIInstance
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIInstanceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIInstance_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIInstance_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIInstance_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIInstance_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_Creact_Cuwp_CIInstance_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_Creact_Cuwp_CIInstance_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_Creact_Cuwp_CIInstance_Start(This,settings)	\
    ( (This)->lpVtbl -> Start(This,settings) ) 

#define __x_ABI_Creact_Cuwp_CIInstance_RegisterModule(This,pModule)	\
    ( (This)->lpVtbl -> RegisterModule(This,pModule) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIInstance_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIInstanceStatics */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIInstanceStatics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A0B3AE3E-3E34-436D-BE4F-603A2C7098DE")
    __x_ABI_Creact_Cuwp_CIInstanceStatics : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ HSTRING bundleFileName,
            /* [out][retval] */ __x_ABI_Creact_Cuwp_CIInstance **instance) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIInstanceStaticsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIInstanceStatics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIInstanceStatics * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Create )( 
            __x_ABI_Creact_Cuwp_CIInstanceStatics * This,
            /* [in] */ HSTRING bundleFileName,
            /* [out][retval] */ __x_ABI_Creact_Cuwp_CIInstance **instance);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIInstanceStaticsVtbl;

    interface __x_ABI_Creact_Cuwp_CIInstanceStatics
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIInstanceStaticsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIInstanceStatics_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIInstanceStatics_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_Creact_Cuwp_CIInstanceStatics_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_Creact_Cuwp_CIInstanceStatics_Create(This,bundleFileName,instance)	\
    ( (This)->lpVtbl -> Create(This,bundleFileName,instance) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIInstanceStatics_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_ReactControl2Eg_0000_0022 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0022_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ReactControl2Eg_0000_0022_v0_0_s_ifspec;

#ifndef ____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIReactControl */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIReactControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BA8CD9D5-78E7-451E-9FA6-FAEAE6B43C52")
    __x_ABI_Creact_Cuwp_CIReactControl : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_JsComponentName( 
            /* [out][retval] */ HSTRING *value) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_JsComponentName( 
            /* [in] */ HSTRING value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_InitialProps( 
            /* [out][retval] */ HSTRING *value) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_InitialProps( 
            /* [in] */ HSTRING value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Instance( 
            /* [out][retval] */ __x_ABI_Creact_Cuwp_CIInstance **value) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Instance( 
            /* [in] */ __x_ABI_Creact_Cuwp_CIInstance *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StartRender( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopRender( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIReactControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_JsComponentName )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [out][retval] */ HSTRING *value);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_JsComponentName )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [in] */ HSTRING value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_InitialProps )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [out][retval] */ HSTRING *value);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_InitialProps )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [in] */ HSTRING value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Instance )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [out][retval] */ __x_ABI_Creact_Cuwp_CIInstance **value);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Instance )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This,
            /* [in] */ __x_ABI_Creact_Cuwp_CIInstance *value);
        
        HRESULT ( STDMETHODCALLTYPE *StartRender )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *StopRender )( 
            __x_ABI_Creact_Cuwp_CIReactControl * This);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIReactControlVtbl;

    interface __x_ABI_Creact_Cuwp_CIReactControl
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIReactControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIReactControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIReactControl_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_Creact_Cuwp_CIReactControl_get_JsComponentName(This,value)	\
    ( (This)->lpVtbl -> get_JsComponentName(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_put_JsComponentName(This,value)	\
    ( (This)->lpVtbl -> put_JsComponentName(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_get_InitialProps(This,value)	\
    ( (This)->lpVtbl -> get_InitialProps(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_put_InitialProps(This,value)	\
    ( (This)->lpVtbl -> put_InitialProps(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_get_Instance(This,value)	\
    ( (This)->lpVtbl -> get_Instance(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_put_Instance(This,value)	\
    ( (This)->lpVtbl -> put_Instance(This,value) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_StartRender(This)	\
    ( (This)->lpVtbl -> StartRender(This) ) 

#define __x_ABI_Creact_Cuwp_CIReactControl_StopRender(This)	\
    ( (This)->lpVtbl -> StopRender(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIReactControl_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__
#define ____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__

/* interface __x_ABI_Creact_Cuwp_CIReactControlFactory */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_Creact_Cuwp_CIReactControlFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B882C924-0A23-4B4A-9088-8FAECB2D5945")
    __x_ABI_Creact_Cuwp_CIReactControlFactory : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ IInspectable *outer,
            /* [out] */ IInspectable **inner,
            /* [out][retval] */ __x_ABI_Creact_Cuwp_CIReactControl **instance) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_Creact_Cuwp_CIReactControlFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_Creact_Cuwp_CIReactControlFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_Creact_Cuwp_CIReactControlFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *CreateInstance )( 
            __x_ABI_Creact_Cuwp_CIReactControlFactory * This,
            /* [in] */ IInspectable *outer,
            /* [out] */ IInspectable **inner,
            /* [out][retval] */ __x_ABI_Creact_Cuwp_CIReactControl **instance);
        
        END_INTERFACE
    } __x_ABI_Creact_Cuwp_CIReactControlFactoryVtbl;

    interface __x_ABI_Creact_Cuwp_CIReactControlFactory
    {
        CONST_VTBL struct __x_ABI_Creact_Cuwp_CIReactControlFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_Creact_Cuwp_CIReactControlFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_Creact_Cuwp_CIReactControlFactory_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_Creact_Cuwp_CIReactControlFactory_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_Creact_Cuwp_CIReactControlFactory_CreateInstance(This,outer,inner,instance)	\
    ( (This)->lpVtbl -> CreateInstance(This,outer,inner,instance) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_Creact_Cuwp_CIReactControlFactory_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HSTRING_UserSize(     unsigned long *, unsigned long            , HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserMarshal(  unsigned long *, unsigned char *, HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserUnmarshal(unsigned long *, unsigned char *, HSTRING * ); 
void                      __RPC_USER  HSTRING_UserFree(     unsigned long *, HSTRING * ); 

unsigned long             __RPC_USER  HSTRING_UserSize64(     unsigned long *, unsigned long            , HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserMarshal64(  unsigned long *, unsigned char *, HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserUnmarshal64(unsigned long *, unsigned char *, HSTRING * ); 
void                      __RPC_USER  HSTRING_UserFree64(     unsigned long *, HSTRING * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


