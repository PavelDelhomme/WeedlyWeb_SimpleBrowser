/****************************************************************************
** Meta object code from reading C++ file 'browserwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../browserwindow.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'browserwindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13BrowserWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN13BrowserWindowE = QtMocHelpers::stringData(
    "BrowserWindow",
    "handleNewWindowTriggered",
    "",
    "handleNewIncognitoWindowTriggered",
    "handleFileOpenTriggered",
    "handleFindActionTriggered",
    "handleShowWindowTriggered",
    "handleWebViewLoadProgress",
    "handleWebViewTitleChanged",
    "title",
    "handleWebActionEnabledChanged",
    "QWebEnginePage::WebAction",
    "action",
    "enabled",
    "handleDevToolsRequested",
    "QWebEnginePage*",
    "source",
    "handleFindTextFinished",
    "QWebEngineFindTextResult",
    "result",
    "handleFavActionTriggered",
    "showFavoritesManager",
    "saveFavoritesFromTree",
    "QTreeWidget*",
    "tree"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN13BrowserWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    0,   93,    2, 0x08,    2 /* Private */,
       4,    0,   94,    2, 0x08,    3 /* Private */,
       5,    0,   95,    2, 0x08,    4 /* Private */,
       6,    0,   96,    2, 0x08,    5 /* Private */,
       7,    1,   97,    2, 0x08,    6 /* Private */,
       8,    1,  100,    2, 0x08,    8 /* Private */,
      10,    2,  103,    2, 0x08,   10 /* Private */,
      14,    1,  108,    2, 0x08,   13 /* Private */,
      17,    1,  111,    2, 0x08,   15 /* Private */,
      20,    0,  114,    2, 0x08,   17 /* Private */,
      21,    0,  115,    2, 0x08,   18 /* Private */,
      22,    1,  116,    2, 0x08,   19 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, 0x80000000 | 11, QMetaType::Bool,   12,   13,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 23,   24,

       0        // eod
};

Q_CONSTINIT const QMetaObject BrowserWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN13BrowserWindowE.offsetsAndSizes,
    qt_meta_data_ZN13BrowserWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN13BrowserWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BrowserWindow, std::true_type>,
        // method 'handleNewWindowTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleNewIncognitoWindowTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleFileOpenTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleFindActionTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleShowWindowTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleWebViewLoadProgress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handleWebViewTitleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handleWebActionEnabledChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage::WebAction, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'handleDevToolsRequested'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QWebEnginePage *, std::false_type>,
        // method 'handleFindTextFinished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QWebEngineFindTextResult &, std::false_type>,
        // method 'handleFavActionTriggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showFavoritesManager'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveFavoritesFromTree'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QTreeWidget *, std::false_type>
    >,
    nullptr
} };

void BrowserWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BrowserWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->handleNewWindowTriggered(); break;
        case 1: _t->handleNewIncognitoWindowTriggered(); break;
        case 2: _t->handleFileOpenTriggered(); break;
        case 3: _t->handleFindActionTriggered(); break;
        case 4: _t->handleShowWindowTriggered(); break;
        case 5: _t->handleWebViewLoadProgress((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->handleWebViewTitleChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->handleWebActionEnabledChanged((*reinterpret_cast< std::add_pointer_t<QWebEnginePage::WebAction>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 8: _t->handleDevToolsRequested((*reinterpret_cast< std::add_pointer_t<QWebEnginePage*>>(_a[1]))); break;
        case 9: _t->handleFindTextFinished((*reinterpret_cast< std::add_pointer_t<QWebEngineFindTextResult>>(_a[1]))); break;
        case 10: _t->handleFavActionTriggered(); break;
        case 11: _t->showFavoritesManager(); break;
        case 12: _t->saveFavoritesFromTree((*reinterpret_cast< std::add_pointer_t<QTreeWidget*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QWebEnginePage* >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QTreeWidget* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *BrowserWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BrowserWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN13BrowserWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int BrowserWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
