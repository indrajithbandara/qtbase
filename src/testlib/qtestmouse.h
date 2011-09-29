/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QTESTMOUSE_H
#define QTESTMOUSE_H

#if 0
// inform syncqt
#pragma qt_no_master_include
#endif

#include <QtTest/qtest_global.h>
#include <QtTest/qtestassert.h>
#include <QtTest/qtestsystem.h>
#include <QtTest/qtestspontaneevent.h>
#include <QtCore/qpoint.h>
#include <QtCore/qstring.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qwidget.h>
#include <QtGui/qwindowsysteminterface_qpa.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

namespace QTest
{
    enum MouseAction { MousePress, MouseRelease, MouseClick, MouseDClick, MouseMove };

    static void mouseEvent(MouseAction action, QWindow *window, Qt::MouseButton button,
                           Qt::KeyboardModifiers stateKey, QPoint pos, int delay=-1)
    {
        QTEST_ASSERT(window);
        extern int Q_TESTLIB_EXPORT defaultMouseDelay();

         static Qt::MouseButton lastButton = Qt::NoButton;

        if (delay == -1 || delay < defaultMouseDelay())
            delay = defaultMouseDelay();
        if (delay > 0)
            QTest::qWait(delay);

        if (pos.isNull())
            pos = window->geometry().center();

        if (action == MouseClick) {
            mouseEvent(MousePress, window, button, stateKey, pos);
            mouseEvent(MouseRelease, window, button, stateKey, pos);
            return;
        }
        QTEST_ASSERT(button == Qt::NoButton || button & Qt::MouseButtonMask);
        QTEST_ASSERT(stateKey == 0 || stateKey & Qt::KeyboardModifierMask);

        stateKey &= static_cast<unsigned int>(Qt::KeyboardModifierMask);


        switch (action)
        {
            case MousePress:
                QWindowSystemInterface::handleMouseEvent(window,pos,window->mapToGlobal(pos),button);
                lastButton = button;
                break;
            case MouseRelease:
                QWindowSystemInterface::handleMouseEvent(window,pos,window->mapToGlobal(pos),Qt::NoButton);
                lastButton = Qt::NoButton;
                break;
            case MouseDClick:
                QWindowSystemInterface::handleMouseEvent(window,pos,window->mapToGlobal(pos),button);
                qWait(10);
                QWindowSystemInterface::handleMouseEvent(window,pos,window->mapToGlobal(pos),Qt::NoButton);
                qWait(20);
                QWindowSystemInterface::handleMouseEvent(window,pos,window->mapToGlobal(pos),button);
                qWait(10);
                QWindowSystemInterface::handleMouseEvent(window,pos,window->mapToGlobal(pos),Qt::NoButton);
                break;
            case MouseMove:
                QWindowSystemInterface::handleMouseEvent(window,pos,window->mapToGlobal(pos),lastButton);
                //QCursor::setPos(window->mapToGlobal(pos));
#ifdef Q_OS_MAC
                QTest::qWait(20);
#else
                qApp->processEvents();
#endif
                return;
            default:
                QTEST_ASSERT(false);
        }
    }

    inline void mousePress(QWindow *window, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                           QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MousePress, window, button, stateKey, pos, delay); }
    inline void mouseRelease(QWindow *window, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                             QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseRelease, window, button, stateKey, pos, delay); }
    inline void mouseClick(QWindow *window, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                           QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseClick, window, button, stateKey, pos, delay); }
    inline void mouseDClick(QWindow *window, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                            QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseDClick, window, button, stateKey, pos, delay); }
    inline void mouseMove(QWindow *window, QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseMove, window, Qt::NoButton, 0, pos, delay); }

#ifdef QT_WIDGETS_LIB
    static void mouseEvent(MouseAction action, QWidget *widget, Qt::MouseButton button,
                           Qt::KeyboardModifiers stateKey, QPoint pos, int delay=-1)
    {
        QTEST_ASSERT(widget);
        extern int Q_TESTLIB_EXPORT defaultMouseDelay();

        if (delay == -1 || delay < defaultMouseDelay())
            delay = defaultMouseDelay();
        if (delay > 0)
            QTest::qWait(delay);

        if (pos.isNull())
            pos = widget->rect().center();

        if (action == MouseClick) {
            mouseEvent(MousePress, widget, button, stateKey, pos);
            mouseEvent(MouseRelease, widget, button, stateKey, pos);
            return;
        }

        QTEST_ASSERT(button == Qt::NoButton || button & Qt::MouseButtonMask);
        QTEST_ASSERT(stateKey == 0 || stateKey & Qt::KeyboardModifierMask);

        stateKey &= static_cast<unsigned int>(Qt::KeyboardModifierMask);

        QMouseEvent me(QEvent::User, QPoint(), Qt::LeftButton, button, stateKey);
        switch (action)
        {
            case MousePress:
                me = QMouseEvent(QEvent::MouseButtonPress, pos, widget->mapToGlobal(pos), button, button, stateKey);
                break;
            case MouseRelease:
                me = QMouseEvent(QEvent::MouseButtonRelease, pos, widget->mapToGlobal(pos), button, 0, stateKey);
                break;
            case MouseDClick:
                me = QMouseEvent(QEvent::MouseButtonDblClick, pos, widget->mapToGlobal(pos), button, button, stateKey);
                break;
            case MouseMove:
                QCursor::setPos(widget->mapToGlobal(pos));
#ifdef Q_OS_MAC
                QTest::qWait(20);
#else
                qApp->processEvents();
#endif
                return;
            default:
                QTEST_ASSERT(false);
        }
        QSpontaneKeyEvent::setSpontaneous(&me);
        if (!qApp->notify(widget, &me)) {
            static const char *mouseActionNames[] =
                { "MousePress", "MouseRelease", "MouseClick", "MouseDClick", "MouseMove" };
            QString warning = QString::fromLatin1("Mouse event \"%1\" not accepted by receiving widget");
            QTest::qWarn(warning.arg(QString::fromLatin1(mouseActionNames[static_cast<int>(action)])).toAscii().data());
        }

    }

    inline void mousePress(QWidget *widget, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                           QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MousePress, widget, button, stateKey, pos, delay); }
    inline void mouseRelease(QWidget *widget, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                             QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseRelease, widget, button, stateKey, pos, delay); }
    inline void mouseClick(QWidget *widget, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                           QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseClick, widget, button, stateKey, pos, delay); }
    inline void mouseDClick(QWidget *widget, Qt::MouseButton button, Qt::KeyboardModifiers stateKey = 0,
                            QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseDClick, widget, button, stateKey, pos, delay); }
    inline void mouseMove(QWidget *widget, QPoint pos = QPoint(), int delay=-1)
    { mouseEvent(MouseMove, widget, Qt::NoButton, 0, pos, delay); }
#endif // QT_WIDGETS_LIB
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTESTMOUSE_H
