/****************************************************************************
** Meta object code from reading C++ file 'addurl.h'
**
** Created: Mon Dec 7 23:22:42 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "addurl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'addurl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AddUrl[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AddUrl[] = {
    "AddUrl\0\0OkButton()\0"
};

const QMetaObject AddUrl::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_AddUrl,
      qt_meta_data_AddUrl, 0 }
};

const QMetaObject *AddUrl::metaObject() const
{
    return &staticMetaObject;
}

void *AddUrl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AddUrl))
        return static_cast<void*>(const_cast< AddUrl*>(this));
    return QDialog::qt_metacast(_clname);
}

int AddUrl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OkButton(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
