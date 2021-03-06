/*
 * File: gchooser.cpp
 * ------------------
 *
 * @author Marty Stepp
 * @version 2019/02/02
 * - destructor now stops event processing
 * @version 2018/08/23
 * - renamed to gchooser.cpp to replace Java version
 * @version 2018/06/28
 * - initial version
 */

#define INTERNAL_INCLUDE 1
#include "gchooser.h"
#define INTERNAL_INCLUDE 1
#include "error.h"
#define INTERNAL_INCLUDE 1
#include "gthread.h"
#define INTERNAL_INCLUDE 1
#include "gwindow.h"
#define INTERNAL_INCLUDE 1
#include "require.h"
#undef INTERNAL_INCLUDE

GChooser::GChooser(QWidget* parent) {
    GThread::runOnQtGuiThread([this, parent]() {
        _iqcomboBox = new _Internal_QComboBox(this, getInternalParent(parent));
    });
    setVisible(false);   // all widgets are not shown until added to a window
}

GChooser::GChooser(const std::initializer_list<std::string>& items, QWidget* parent) {
    GThread::runOnQtGuiThread([this, parent]() {
        _iqcomboBox = new _Internal_QComboBox(this, getInternalParent(parent));
    });
    addItems(items);
    setVisible(false);   // all widgets are not shown until added to a window
}

GChooser::GChooser(const Vector<std::string>& items, QWidget* parent) {
    GThread::runOnQtGuiThread([this, parent]() {
        _iqcomboBox = new _Internal_QComboBox(this, getInternalParent(parent));
    });
    addItems(items);
    setVisible(false);   // all widgets are not shown until added to a window
}

GChooser::~GChooser() {
    // TODO: delete _iqcomboBox;
    _iqcomboBox->_gchooser = nullptr;
    _iqcomboBox = nullptr;
}

void GChooser::addItem(const std::string& item) {
    require::nonEmpty(item, "GChooser::addItem", "item");
    GThread::runOnQtGuiThread([this, item]() {
        _iqcomboBox->addItem(QString::fromStdString(item));
    });
}

void GChooser::addItems(const std::initializer_list<std::string>& items) {
    GThread::runOnQtGuiThread([this, &items]() {
        for (const std::string& item : items) {
            if (!item.empty()) {
                _iqcomboBox->addItem(QString::fromStdString(item));
            }
        }
    });
}

void GChooser::addItems(const Vector<std::string>& items) {
    GThread::runOnQtGuiThread([this, &items]() {
        for (const std::string& item : items) {
            if (!item.empty()) {
                _iqcomboBox->addItem(QString::fromStdString(item));
            }
        }
    });
}

void GChooser::checkIndex(const std::string& member, int index, int min, int max) const {
    if (max < 0) {
        max = size() - 1;
    }
    require::inRange(index, min, max, member, "index");
}

void GChooser::clearItems() {
    GThread::runOnQtGuiThread([this]() {
        _iqcomboBox->clear();
    });
}

std::string GChooser::getActionCommand() const {
    if (_actionCommand.empty()) {
        return getSelectedItem();
    } else {
        return _actionCommand;
    }
}

_Internal_QWidget* GChooser::getInternalWidget() const {
    return _iqcomboBox;
}

std::string GChooser::getItem(int index) const {
    checkIndex("GChooser::getItem", index);
    return _iqcomboBox->itemText(index).toStdString();
}

int GChooser::getItemCount() const {
    return _iqcomboBox->count();
}

int GChooser::getSelectedIndex() const {
    return _iqcomboBox->currentIndex();
}

std::string GChooser::getSelectedItem() const {
    return _iqcomboBox->currentText().toStdString();
}

std::string GChooser::getType() const {
    return "GChooser";
}

QWidget* GChooser::getWidget() const {
    return static_cast<QWidget*>(_iqcomboBox);
}

bool GChooser::isEditable() const {
    return _iqcomboBox->isEditable();
}

bool GChooser::isEmpty() const {
    return getItemCount() == 0;
}

void GChooser::removeActionListener() {
    removeEventListener("change");
}

void GChooser::setActionListener(GEventListener func) {
    setEventListener("change", func);
}

void GChooser::setActionListener(GEventListenerVoid func) {
    setEventListener("change", func);
}

void GChooser::setItem(int index, const std::string& item) {
    checkIndex("GChooser::setItem", index);
    GThread::runOnQtGuiThread([this, index, item]() {
        _iqcomboBox->setItemText(index, QString::fromStdString(item));
    });
}

void GChooser::setSelectedIndex(int index) {
    checkIndex("GChooser::setSelectedIndex", index);
    GThread::runOnQtGuiThread([this, index]() {
        _iqcomboBox->setCurrentIndex(index);
    });
}

void GChooser::setEditable(bool editable) {
    GThread::runOnQtGuiThread([this, editable]() {
        _iqcomboBox->setEditable(editable);
    });
}

void GChooser::setSelectedItem(const std::string& item) {
    for (int i = 0, len = getItemCount(); i < len; i++) {
        std::string thisItem = _iqcomboBox->itemText(i).toStdString();
        if (thisItem == item) {
            setSelectedIndex(i);
            break;
        }
    }
}

int GChooser::size() const {
    return _iqcomboBox->count();
}


_Internal_QComboBox::_Internal_QComboBox(GChooser* gchooser, QWidget* parent)
        : QComboBox(parent),
          _gchooser(gchooser) {
    require::nonNull(gchooser, "_Internal_QComboBox::constructor");
    setObjectName(QString::fromStdString("_Internal_QComboBox_" + std::to_string(gchooser->getID())));
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(handleChange()));
}

void _Internal_QComboBox::handleChange() {
    if (!_gchooser) {
        return;
    }
    GEvent changeEvent(
                /* class  */ CHANGE_EVENT,
                /* type   */ STATE_CHANGED,
                /* name   */ "change",
                /* source */ _gchooser);
    changeEvent.setActionCommand(_gchooser->getActionCommand());
    _gchooser->fireEvent(changeEvent);
}

QSize _Internal_QComboBox::sizeHint() const {
    if (hasPreferredSize()) {
        return getPreferredSize();
    } else {
        return QComboBox::sizeHint();
    }
}

#ifdef SPL_PRECOMPILE_QT_MOC_FILES
#define INTERNAL_INCLUDE 1
#include "moc_gchooser.cpp"   // speeds up compilation of auto-generated Qt files
#undef INTERNAL_INCLUDE
#endif // SPL_PRECOMPILE_QT_MOC_FILES
