/****************************************************************************
** Meta object code from reading C++ file 'stereo_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../car-gui/stereo_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stereo_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_StereoWindow_t {
    QByteArrayData data[7];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StereoWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StereoWindow_t qt_meta_stringdata_StereoWindow = {
    {
QT_MOC_LITERAL(0, 0, 12), // "StereoWindow"
QT_MOC_LITERAL(1, 13, 28), // "on_frame_slider_valueChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 5), // "value"
QT_MOC_LITERAL(4, 49, 33), // "on_show_features_checkbox_tog..."
QT_MOC_LITERAL(5, 83, 34), // "on_match_features_checkbox_to..."
QT_MOC_LITERAL(6, 118, 7) // "checked"

    },
    "StereoWindow\0on_frame_slider_valueChanged\0"
    "\0value\0on_show_features_checkbox_toggled\0"
    "on_match_features_checkbox_toggled\0"
    "checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StereoWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x08 /* Private */,
       4,    1,   32,    2, 0x08 /* Private */,
       5,    1,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    6,

       0        // eod
};

void StereoWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        StereoWindow *_t = static_cast<StereoWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_frame_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_show_features_checkbox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->on_match_features_checkbox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject StereoWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_StereoWindow.data,
      qt_meta_data_StereoWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *StereoWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StereoWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_StereoWindow.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int StereoWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
