/****************************************************************************
** Meta object code from reading C++ file 'camera_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../car-gui/camera-window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camera_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CameraWindow_t {
    QByteArrayData data[18];
    char stringdata0[388];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CameraWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CameraWindow_t qt_meta_stringdata_CameraWindow = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CameraWindow"
QT_MOC_LITERAL(1, 13, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 17), // "process_one_frame"
QT_MOC_LITERAL(4, 56, 11), // "fps_changed"
QT_MOC_LITERAL(5, 68, 3), // "fps"
QT_MOC_LITERAL(6, 72, 23), // "on_webcamButton_clicked"
QT_MOC_LITERAL(7, 96, 33), // "on_brightness_slider_valueCha..."
QT_MOC_LITERAL(8, 130, 5), // "value"
QT_MOC_LITERAL(9, 136, 31), // "on_contrast_slider_valueChanged"
QT_MOC_LITERAL(10, 168, 26), // "on_hue_slider_valueChanged"
QT_MOC_LITERAL(11, 195, 33), // "on_saturation_slider_valueCha..."
QT_MOC_LITERAL(12, 229, 44), // "on_resolutions_combo_box_curr..."
QT_MOC_LITERAL(13, 274, 5), // "index"
QT_MOC_LITERAL(14, 280, 34), // "on_video_device_currentTextCh..."
QT_MOC_LITERAL(15, 315, 4), // "arg1"
QT_MOC_LITERAL(16, 320, 30), // "on_take_picture_button_clicked"
QT_MOC_LITERAL(17, 351, 36) // "on_video_device_2_currentText..."

    },
    "CameraWindow\0on_actionExit_triggered\0"
    "\0process_one_frame\0fps_changed\0fps\0"
    "on_webcamButton_clicked\0"
    "on_brightness_slider_valueChanged\0"
    "value\0on_contrast_slider_valueChanged\0"
    "on_hue_slider_valueChanged\0"
    "on_saturation_slider_valueChanged\0"
    "on_resolutions_combo_box_currentIndexChanged\0"
    "index\0on_video_device_currentTextChanged\0"
    "arg1\0on_take_picture_button_clicked\0"
    "on_video_device_2_currentTextChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CameraWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    1,   76,    2, 0x08 /* Private */,
       6,    0,   79,    2, 0x08 /* Private */,
       7,    1,   80,    2, 0x08 /* Private */,
       9,    1,   83,    2, 0x08 /* Private */,
      10,    1,   86,    2, 0x08 /* Private */,
      11,    1,   89,    2, 0x08 /* Private */,
      12,    1,   92,    2, 0x08 /* Private */,
      14,    1,   95,    2, 0x08 /* Private */,
      16,    0,   98,    2, 0x08 /* Private */,
      17,    1,   99,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

void CameraWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CameraWindow *_t = static_cast<CameraWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_actionExit_triggered(); break;
        case 1: _t->process_one_frame(); break;
        case 2: _t->fps_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_webcamButton_clicked(); break;
        case 4: _t->on_brightness_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_contrast_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_hue_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_saturation_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_resolutions_combo_box_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->on_video_device_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->on_take_picture_button_clicked(); break;
        case 11: _t->on_video_device_2_currentTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CameraWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CameraWindow.data,
      qt_meta_data_CameraWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CameraWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CameraWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CameraWindow.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CameraWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
