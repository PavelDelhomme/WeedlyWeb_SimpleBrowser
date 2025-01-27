/****************************************************************************
** Meta object code from reading C++ file 'webview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../webview.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'webview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN7WebViewE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN7WebViewE = QtMocHelpers::stringData(
    "WebView",
    "webActionEnabledChanged",
    "",
    "QWebEnginePage::WebAction",
    "webAction",
    "enabled",
    "favIconChanged",
    "icon",
    "devToolsRequested",
    "QWebEnginePage*",
    "source",
    "handleCertificateError",
    "QWebEngineCertificateError",
    "error",
    "handleAuthenticationRequired",
    "requestUrl",
    "QAuthenticator*",
    "auth",
    "handlePermissionRequested",
    "QWebEnginePermission",
    "permission",
    "handleProxyAuthenticationRequired",
    "proxyHost",
    "handleRegisterProtocolHandlerRequested",
    "QWebEngineRegisterProtocolHandlerRequest",
    "request",
    "handleFileSystemAccessRequested",
    "QWebEngineFileSystemAccessRequest",
    "handleWebAuthUxRequested",
    "QWebEngineWebAuthUxRequest*",
    "handleImageAnimationPolicyChange",
    "QWebEngineSettings::ImageAnimationPolicy",
    "policy"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN7WebViewE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,   80,    2, 0x06,    1 /* Public */,
       6,    1,   85,    2, 0x06,    4 /* Public */,
       8,    1,   88,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    1,   91,    2, 0x08,    8 /* Private */,
      14,    2,   94,    2, 0x08,   10 /* Private */,
      18,    1,   99,    2, 0x08,   13 /* Private */,
      21,    3,  102,    2, 0x08,   15 /* Private */,
      23,    1,  109,    2, 0x08,   19 /* Private */,
      26,    1,  112,    2, 0x08,   21 /* Private */,
      28,    1,  115,    2, 0x08,   23 /* Private */,
      30,    1,  118,    2, 0x08,   25 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Bool,    4,    5,
    QMetaType::Void, QMetaType::QIcon,    7,
    QMetaType::Void, 0x80000000 | 9,   10,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 16,   15,   17,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 16, QMetaType::QString,   15,   17,   22,
    QMetaType::Void, 0x80000000 | 24,   25,
    QMetaType::Void, 0x80000000 | 27,   25,
    QMetaType::Void, 0x80000000 | 29,   25,
    QMetaType::Void, 0x80000000 | 31,   32,

       0        // eod
};

Q_CONSTINIT const QMetaObject WebView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWebEngineView::staticMetaObject>(),
    qt_meta_stringdata_ZN7WebViewE.offsetsAndSizes,
    qt_meta_data_ZN7WebViewE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN7WebViewE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WebView, std::true_type>,
        // method 'webActionEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage::WebAction, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'favIconChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QIcon &, std::false_type>,
        // method 'devToolsRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage *, std::false_type>,
        // method 'handleCertificateError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEngineCertificateError, std::false_type>,
        // method 'handleAuthenticationRequired'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAuthenticator *, std::false_type>,
        // method 'handlePermissionRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePermission, std::false_type>,
        // method 'handleProxyAuthenticationRequired'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAuthenticator *, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handleRegisterProtocolHandlerRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEngineRegisterProtocolHandlerRequest, std::false_type>,
        // method 'handleFileSystemAccessRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEngineFileSystemAccessRequest, std::false_type>,
        // method 'handleWebAuthUxRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEngineWebAuthUxRequest *, std::false_type>,
        // method 'handleImageAnimationPolicyChange'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEngineSettings::ImageAnimationPolicy, std::false_type>
    >,
    nullptr
} };

void WebView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WebView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->webActionEnabledChanged((*reinterpret_cast< std::add_pointer_t<QWebEnginePage::WebAction>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 1: _t->favIconChanged((*reinterpret_cast< std::add_pointer_t<QIcon>>(_a[1]))); break;
        case 2: _t->devToolsRequested((*reinterpret_cast< std::add_pointer_t<QWebEnginePage*>>(_a[1]))); break;
        case 3: _t->handleCertificateError((*reinterpret_cast< std::add_pointer_t<QWebEngineCertificateError>>(_a[1]))); break;
        case 4: _t->handleAuthenticationRequired((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QAuthenticator*>>(_a[2]))); break;
        case 5: _t->handlePermissionRequested((*reinterpret_cast< std::add_pointer_t<QWebEnginePermission>>(_a[1]))); break;
        case 6: _t->handleProxyAuthenticationRequired((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QAuthenticator*>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 7: _t->handleRegisterProtocolHandlerRequested((*reinterpret_cast< std::add_pointer_t<QWebEngineRegisterProtocolHandlerRequest>>(_a[1]))); break;
        case 8: _t->handleFileSystemAccessRequested((*reinterpret_cast< std::add_pointer_t<QWebEngineFileSystemAccessRequest>>(_a[1]))); break;
        case 9: _t->handleWebAuthUxRequested((*reinterpret_cast< std::add_pointer_t<QWebEngineWebAuthUxRequest*>>(_a[1]))); break;
        case 10: _t->handleImageAnimationPolicyChange((*reinterpret_cast< std::add_pointer_t<QWebEngineSettings::ImageAnimationPolicy>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEnginePage* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEngineCertificateError >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEngineWebAuthUxRequest* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (WebView::*)(QWebEnginePage::WebAction , bool );
            if (_q_method_type _q_method = &WebView::webActionEnabledChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (WebView::*)(const QIcon & );
            if (_q_method_type _q_method = &WebView::favIconChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (WebView::*)(QWebEnginePage * );
            if (_q_method_type _q_method = &WebView::devToolsRequested; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *WebView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WebView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN7WebViewE.stringdata0))
        return static_cast<void*>(this);
    return QWebEngineView::qt_metacast(_clname);
}

int WebView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEngineView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void WebView::webActionEnabledChanged(QWebEnginePage::WebAction _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WebView::favIconChanged(const QIcon & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WebView::devToolsRequested(QWebEnginePage * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
