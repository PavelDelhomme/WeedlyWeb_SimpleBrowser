/****************************************************************************
** Meta object code from reading C++ file 'tabwidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../tabwidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tabwidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9TabWidgetE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN9TabWidgetE = QtMocHelpers::stringData(
    "TabWidget",
    "linkHovered",
    "",
    "link",
    "loadProgress",
    "progress",
    "titleChanged",
    "title",
    "urlChanged",
    "url",
    "favIconChanged",
    "icon",
    "webActionEnabledChanged",
    "QWebEnginePage::WebAction",
    "action",
    "enabled",
    "devToolsRequested",
    "QWebEnginePage*",
    "source",
    "findTextFinished",
    "QWebEngineFindTextResult",
    "result",
    "setUrl",
    "triggerWebPageAction",
    "createTab",
    "WebView*",
    "createBackgroundTab",
    "closeTab",
    "index",
    "nextTab",
    "previousTab",
    "handleCurrentChanged",
    "handleContextMenuRequested",
    "pos",
    "cloneTab",
    "closeOtherTabs",
    "reloadAllTabs",
    "reloadTab"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN9TabWidgetE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  140,    2, 0x06,    1 /* Public */,
       4,    1,  143,    2, 0x06,    3 /* Public */,
       6,    1,  146,    2, 0x06,    5 /* Public */,
       8,    1,  149,    2, 0x06,    7 /* Public */,
      10,    1,  152,    2, 0x06,    9 /* Public */,
      12,    2,  155,    2, 0x06,   11 /* Public */,
      16,    1,  160,    2, 0x06,   14 /* Public */,
      19,    1,  163,    2, 0x06,   16 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      22,    1,  166,    2, 0x0a,   18 /* Public */,
      23,    1,  169,    2, 0x0a,   20 /* Public */,
      24,    0,  172,    2, 0x0a,   22 /* Public */,
      26,    0,  173,    2, 0x0a,   23 /* Public */,
      27,    1,  174,    2, 0x0a,   24 /* Public */,
      29,    0,  177,    2, 0x0a,   26 /* Public */,
      30,    0,  178,    2, 0x0a,   27 /* Public */,
      31,    1,  179,    2, 0x08,   28 /* Private */,
      32,    1,  182,    2, 0x08,   30 /* Private */,
      34,    1,  185,    2, 0x08,   32 /* Private */,
      35,    1,  188,    2, 0x08,   34 /* Private */,
      36,    0,  191,    2, 0x08,   36 /* Private */,
      37,    1,  192,    2, 0x08,   37 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QUrl,    9,
    QMetaType::Void, QMetaType::QIcon,   11,
    QMetaType::Void, 0x80000000 | 13, QMetaType::Bool,   14,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 20,   21,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,    9,
    QMetaType::Void, 0x80000000 | 13,   14,
    0x80000000 | 25,
    0x80000000 | 25,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void, QMetaType::QPoint,   33,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   28,

       0        // eod
};

Q_CONSTINIT const QMetaObject TabWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QTabWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN9TabWidgetE.offsetsAndSizes,
    qt_meta_data_ZN9TabWidgetE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN9TabWidgetE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TabWidget, std::true_type>,
        // method 'linkHovered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'loadProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'titleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'urlChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'favIconChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QIcon &, std::false_type>,
        // method 'webActionEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage::WebAction, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'devToolsRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage *, std::false_type>,
        // method 'findTextFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QWebEngineFindTextResult &, std::false_type>,
        // method 'setUrl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QUrl &, std::false_type>,
        // method 'triggerWebPageAction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage::WebAction, std::false_type>,
        // method 'createTab'
        QtPrivate::TypeAndForceComplete<WebView *, std::false_type>,
        // method 'createBackgroundTab'
        QtPrivate::TypeAndForceComplete<WebView *, std::false_type>,
        // method 'closeTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'nextTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'previousTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleCurrentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handleContextMenuRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'cloneTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'closeOtherTabs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'reloadAllTabs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reloadTab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void TabWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TabWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->linkHovered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->loadProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->titleChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->urlChanged((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 4: _t->favIconChanged((*reinterpret_cast< std::add_pointer_t<QIcon>>(_a[1]))); break;
        case 5: _t->webActionEnabledChanged((*reinterpret_cast< std::add_pointer_t<QWebEnginePage::WebAction>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->devToolsRequested((*reinterpret_cast< std::add_pointer_t<QWebEnginePage*>>(_a[1]))); break;
        case 7: _t->findTextFinished((*reinterpret_cast< std::add_pointer_t<QWebEngineFindTextResult>>(_a[1]))); break;
        case 8: _t->setUrl((*reinterpret_cast< std::add_pointer_t<QUrl>>(_a[1]))); break;
        case 9: _t->triggerWebPageAction((*reinterpret_cast< std::add_pointer_t<QWebEnginePage::WebAction>>(_a[1]))); break;
        case 10: { WebView* _r = _t->createTab();
            if (_a[0]) *reinterpret_cast< WebView**>(_a[0]) = std::move(_r); }  break;
        case 11: { WebView* _r = _t->createBackgroundTab();
            if (_a[0]) *reinterpret_cast< WebView**>(_a[0]) = std::move(_r); }  break;
        case 12: _t->closeTab((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->nextTab(); break;
        case 14: _t->previousTab(); break;
        case 15: _t->handleCurrentChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->handleContextMenuRequested((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 17: _t->cloneTab((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->closeOtherTabs((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->reloadAllTabs(); break;
        case 20: _t->reloadTab((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEnginePage* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEngineFindTextResult >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (TabWidget::*)(const QString & );
            if (_q_method_type _q_method = &TabWidget::linkHovered; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (TabWidget::*)(int );
            if (_q_method_type _q_method = &TabWidget::loadProgress; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (TabWidget::*)(const QString & );
            if (_q_method_type _q_method = &TabWidget::titleChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (TabWidget::*)(const QUrl & );
            if (_q_method_type _q_method = &TabWidget::urlChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (TabWidget::*)(const QIcon & );
            if (_q_method_type _q_method = &TabWidget::favIconChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (TabWidget::*)(QWebEnginePage::WebAction , bool );
            if (_q_method_type _q_method = &TabWidget::webActionEnabledChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (TabWidget::*)(QWebEnginePage * );
            if (_q_method_type _q_method = &TabWidget::devToolsRequested; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _q_method_type = void (TabWidget::*)(const QWebEngineFindTextResult & );
            if (_q_method_type _q_method = &TabWidget::findTextFinished; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *TabWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TabWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN9TabWidgetE.stringdata0))
        return static_cast<void*>(this);
    return QTabWidget::qt_metacast(_clname);
}

int TabWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    return _id;
}

// SIGNAL 0
void TabWidget::linkHovered(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TabWidget::loadProgress(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TabWidget::titleChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TabWidget::urlChanged(const QUrl & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void TabWidget::favIconChanged(const QIcon & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void TabWidget::webActionEnabledChanged(QWebEnginePage::WebAction _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void TabWidget::devToolsRequested(QWebEnginePage * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void TabWidget::findTextFinished(const QWebEngineFindTextResult & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
