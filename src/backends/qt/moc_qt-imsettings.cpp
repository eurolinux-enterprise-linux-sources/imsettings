/****************************************************************************
** Meta object code from reading C++ file 'qt-imsettings.h'
**
** Created: Tue Mar 23 12:01:49 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qt-imsettings.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qt-imsettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_IMSettingsQt[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,   34,   29,   13, 0x08,
      84,   59,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_IMSettingsQt[] = {
    "IMSettingsQt\0\0disconnected()\0bool\0"
    "module\0ChangeTo(QString)\0"
    "name,old_owner,new_owner\0"
    "NameOwnerChanged(QString,QString,QString)\0"
};

const QMetaObject IMSettingsQt::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_IMSettingsQt,
      qt_meta_data_IMSettingsQt, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &IMSettingsQt::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *IMSettingsQt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *IMSettingsQt::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IMSettingsQt))
        return static_cast<void*>(const_cast< IMSettingsQt*>(this));
    return QObject::qt_metacast(_clname);
}

int IMSettingsQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: disconnected(); break;
        case 1: { bool _r = ChangeTo((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: NameOwnerChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void IMSettingsQt::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
