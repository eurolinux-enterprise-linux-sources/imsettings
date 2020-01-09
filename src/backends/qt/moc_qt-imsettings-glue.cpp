/****************************************************************************
** Meta object code from reading C++ file 'qt-imsettings-glue.h'
**
** Created: Tue Mar 23 12:01:49 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qt-imsettings-glue.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qt-imsettings-glue.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_IMSettingsQtDBusAdaptor[] = {

 // content:
       4,       // revision
       0,       // classname
       2,   14, // classinfo
       1,   18, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // classinfo: key, value
      49,   24,
     270,   65,

 // slots: signature, parameters, type, tag, flags
     303,  296,  291,  290, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_IMSettingsQtDBusAdaptor[] = {
    "IMSettingsQtDBusAdaptor\0"
    "com.redhat.imsettings.Qt\0D-Bus Interface\0"
    "  <interface name=\"com.redhat.imsettings.Qt\">\n    <method name=\"Ch"
    "angeTo\">\n      <arg direction=\"in\" type=\"s\" name=\"module\"/>\n "
    "     <arg direction=\"out\" type=\"b\" name=\"ret\"/>\n    </method>\n"
    "  </interface>\n\0"
    "D-Bus Introspection\0\0bool\0module\0"
    "ChangeTo(QString)\0"
};

const QMetaObject IMSettingsQtDBusAdaptor::staticMetaObject = {
    { &QDBusAbstractAdaptor::staticMetaObject, qt_meta_stringdata_IMSettingsQtDBusAdaptor,
      qt_meta_data_IMSettingsQtDBusAdaptor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &IMSettingsQtDBusAdaptor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *IMSettingsQtDBusAdaptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *IMSettingsQtDBusAdaptor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IMSettingsQtDBusAdaptor))
        return static_cast<void*>(const_cast< IMSettingsQtDBusAdaptor*>(this));
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int IMSettingsQtDBusAdaptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = ChangeTo((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
