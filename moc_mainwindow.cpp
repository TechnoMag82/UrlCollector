/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Jan 4 12:31:15 2010
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      20,   11,   11,   11, 0x08,
      38,   33,   11,   11, 0x08,
      64,   11,   11,   11, 0x08,
      73,   11,   11,   11, 0x08,
      83,   11,   11,   11, 0x08,
      93,   11,   11,   11, 0x08,
     110,   11,   11,   11, 0x08,
     131,  126,   11,   11, 0x08,
     150,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0About()\0execAddUrl()\0item\0"
    "getInfo(QListWidgetItem*)\0delUrl()\0"
    "gotoUrl()\0Options()\0setSearchFocus()\0"
    "showFavorites()\0text\0getDBItem(QString)\0"
    "initApp()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: About(); break;
        case 1: execAddUrl(); break;
        case 2: getInfo((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: delUrl(); break;
        case 4: gotoUrl(); break;
        case 5: Options(); break;
        case 6: setSearchFocus(); break;
        case 7: showFavorites(); break;
        case 8: getDBItem((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: initApp(); break;
        }
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
