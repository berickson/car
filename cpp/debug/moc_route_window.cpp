/****************************************************************************
** Meta object code from reading C++ file 'route_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../car-gui/route-window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'route_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RouteWindow_t {
    QByteArrayData data[14];
    char stringdata0[292];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RouteWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RouteWindow_t qt_meta_stringdata_RouteWindow = {
    {
QT_MOC_LITERAL(0, 0, 11), // "RouteWindow"
QT_MOC_LITERAL(1, 12, 34), // "on_track_list_itemSelectionCh..."
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 34), // "on_route_list_itemSelectionCh..."
QT_MOC_LITERAL(4, 83, 32), // "on_run_list_itemSelectionChanged"
QT_MOC_LITERAL(5, 116, 12), // "get_k_smooth"
QT_MOC_LITERAL(6, 129, 13), // "show_run_data"
QT_MOC_LITERAL(7, 143, 6), // "Route&"
QT_MOC_LITERAL(8, 150, 3), // "run"
QT_MOC_LITERAL(9, 154, 32), // "on_run_data_itemSelectionChanged"
QT_MOC_LITERAL(10, 187, 35), // "on_run_position_slider_valueC..."
QT_MOC_LITERAL(11, 223, 5), // "value"
QT_MOC_LITERAL(12, 229, 31), // "on_folder_picker_button_clicked"
QT_MOC_LITERAL(13, 261, 30) // "on_synch_folder_button_clicked"

    },
    "RouteWindow\0on_track_list_itemSelectionChanged\0"
    "\0on_route_list_itemSelectionChanged\0"
    "on_run_list_itemSelectionChanged\0"
    "get_k_smooth\0show_run_data\0Route&\0run\0"
    "on_run_data_itemSelectionChanged\0"
    "on_run_position_slider_valueChanged\0"
    "value\0on_folder_picker_button_clicked\0"
    "on_synch_folder_button_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RouteWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    0,   62,    2, 0x08 /* Private */,
       6,    1,   63,    2, 0x08 /* Private */,
       9,    0,   66,    2, 0x08 /* Private */,
      10,    1,   67,    2, 0x08 /* Private */,
      12,    0,   70,    2, 0x08 /* Private */,
      13,    0,   71,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Double,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RouteWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RouteWindow *_t = static_cast<RouteWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_track_list_itemSelectionChanged(); break;
        case 1: _t->on_route_list_itemSelectionChanged(); break;
        case 2: _t->on_run_list_itemSelectionChanged(); break;
        case 3: { double _r = _t->get_k_smooth();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->show_run_data((*reinterpret_cast< Route(*)>(_a[1]))); break;
        case 5: _t->on_run_data_itemSelectionChanged(); break;
        case 6: _t->on_run_position_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_folder_picker_button_clicked(); break;
        case 8: _t->on_synch_folder_button_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject RouteWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_RouteWindow.data,
      qt_meta_data_RouteWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RouteWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RouteWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RouteWindow.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int RouteWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
