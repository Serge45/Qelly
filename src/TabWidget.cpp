/*****************************************************************************
 * TabWidget.cpp
 *
 * Created: 05/10 2011 by uranusjr
 *
 * Copyright 2013 uranusjr. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/

#include "TabWidget.h"
#include <QMouseEvent>
#include <QShortcut>
#include <QSignalMapper>
#include <QTabBar>
#include "SharedPreferences.h"
#include "View.h"

namespace UJ
{

namespace Qelly
{

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    setStyleSheet("QTabBar::tab { width: 150px; }");
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    tabBar()->setAutoFillBackground(true);
    setAutoFillBackground(true);
    setDocumentMode(true);
    setTabsClosable(true);
    setMovable(true);
    setUsesScrollButtons(true);
    updateBackground();

    tabBar()->installEventFilter(this);
    connect(tabBar(), SIGNAL(tabMoved(int,int)), SLOT(onTabMoved(int,int)));

    QSignalMapper *shortcutMapper = new QSignalMapper(this);
    connect(shortcutMapper, SIGNAL(mapped(int)), SLOT(goToTabForKey(int)));
    for (int key = Qt::Key_0; key <= Qt::Key_9; key++)
    {
        QShortcut *shortcut =
                new QShortcut(QKeySequence(UJ::ModModifier | key), this);
        shortcut->setContext(Qt::WindowShortcut);
        shortcutMapper->setMapping(shortcut, key);
        shortcutMapper->connect(shortcut, SIGNAL(activated()), SLOT(map()));
    }
}

int TabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label)
{
    preAddTab(widget);
    int result = QTabWidget::addTab(widget, icon, label);
    postAddTab(result, label);
    return result;
}

int TabWidget::addTab(QWidget *widget, const QString &label)
{
    preAddTab(widget);
    int result = QTabWidget::addTab(widget, label);
    postAddTab(result, label);
    return result;
}

void TabWidget::updateBackground()
{
    QPalette p;
    p.setBrush(QPalette::Background,
               SharedPreferences::sharedInstance()->backgroundColor());
    setPalette(p);
}

void TabWidget::closeTab(int index)
{
    QWidget *w = widget(index);
    removeTab(index);
    refreshTabText(index);
    w->deleteLater();
}

bool TabWidget::eventFilter(QObject *obj, QEvent *e)
{
    static int middlePressedTab = -1;
    QWidget *w = qobject_cast<QWidget *>(obj);
    if (obj == tabBar())
    {
        switch (e->type())
        {
        case QEvent::MouseButtonPress:
        {
            QMouseEvent *me = static_cast<QMouseEvent *>(e);
            if (me->button() == Qt::MiddleButton)
                middlePressedTab = tabBar()->tabAt(me->pos());
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent *me = static_cast<QMouseEvent *>(e);
            if (me->button() == Qt::MiddleButton)
            {
                int index = tabBar()->tabAt(me->pos());
                if (index != -1 && index == middlePressedTab)
                    emit tabCloseRequested(index);
                middlePressedTab = -1;
            }
            break;
        }
        default:
            break;
        }
    }
    else if (w && indexOf(w) >= 0)  // w is one of the tabs
    {
        switch (e->type())
        {
        case QEvent::Resize:
        {
            QSize sh = w->sizeHint();
            if (sh.isValid())
            {
                sh.rheight() += tabBar()->height();
                setMinimumSize(sh);
            }
            break;
        }
        default:
            break;
        }
    }
    return false;
}

void TabWidget::onTabMoved(int from, int to)
{
    int start = from;
    int end = to;
    if (start > end)
    {
        int t = end;
        end = start;
        start = t;
    }
    refreshTabText(start, end + 1);
}

void TabWidget::refreshTabText(int start, int end)
{
    if (end < start)
        end = count();
    for (int i = start; i < end; i++)
        setTabText(i, tabText(i));
}

void TabWidget::goToTabForKey(int key)
{
    int i = key - Qt::Key_0;
    if (i == 0)
        i = 10;
    setCurrentIndex(i - 1);
}

void TabWidget::preAddTab(QWidget *w)
{
    setUpdatesEnabled(false);
    w->setParent(this);
    w->installEventFilter(this);
}

void TabWidget::postAddTab(int index, const QString &label)
{
    setTabText(index, label);
    setCurrentIndex(index);
    setUpdatesEnabled(true);
}

}   // namespace Qelly

}   // namespace UJ
