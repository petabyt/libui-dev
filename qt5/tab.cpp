#include "uipriv_qt5.hpp"

#include <QTabWidget>
#include <QLayout>

struct uiTab : public uiQt5Control {};

static void tabInsertAt(QTabWidget *tabWidget, const char *name, int n, uiControl *child)
{
	auto obj = uiValidateAndCastObjTo<QObject>(child);

	if (auto layout = qobject_cast<QLayout*>(obj)) {
		auto widget = new QWidget;
		widget->setLayout(layout);
		tabWidget->insertTab(n, widget, QString::fromUtf8(name));
	} else if (auto widget = qobject_cast<QWidget*>(obj)) {
		tabWidget->insertTab(n, widget, QString::fromUtf8(name));
	} else {
		qWarning("object is neither layout nor widget");
	}
}

void uiTabAppend(uiTab *t, const char *name, uiControl *child)
{
	if (auto tabWidget = uiValidateAndCastObjTo<QTabWidget>(t)) {
		tabInsertAt(tabWidget,name,tabWidget->count(),child);
	}
}

void uiTabInsertAt(uiTab *t, const char *name, int n, uiControl *child)
{
	if (auto tabWidget = uiValidateAndCastObjTo<QTabWidget>(t)) {
		tabInsertAt(tabWidget,name,qBound<int>(0, n, tabWidget->count()),child);
	}
}

// reminder: badly named function, is remove not really a delete
void uiTabDelete(uiTab *t, int n)
{
	if (auto tabWidget = uiValidateAndCastObjTo<QTabWidget>(t)) {
		int i = qBound<int>(0, n, tabWidget->count());
		if (i != n) {
			qWarning("Bad index: %llu", (unsigned long long)n);
			return;
		}
		auto widget = tabWidget->widget(i);
		tabWidget->removeTab(i);
		// check if it's a internal temp container widget we created
		if (uiFindQt5ControlForQObject(widget) == nullptr) {
			// widget is temp we created used as a container, separate the layout and then delete it

			// problem once a layout assigned to widget, it's not so easy to get it back..?
			// one solution is to recreate the layout, but this needs some thought..
			// another solution is to never expose "layout" (uiBox), but always wrap them in containers..
			qWarning("FIXME: can't separate layout from temp widget");
//			delete widget;
		}
	}
}

int uiTabNumPages(uiTab *t)
{
	if (auto tabWidget = uiValidateAndCastObjTo<QTabWidget>(t)) {
		return tabWidget->count();
	}
	return 0;
}

int uiTabMargined(uiTab *t, int n)
{
	if (auto tabWidget = uiValidateAndCastObjTo<QTabWidget>(t)) {
		QMargins s = tabWidget->widget(n)->contentsMargins();
		return s.top() > 0;
	}
	return 0;
}

void uiTabSetMargined(uiTab *t, int n, int margined)
{
	if (auto tabWidget = uiValidateAndCastObjTo<QTabWidget>(t)) {
		if(margined){
			tabWidget->widget(n)->setContentsMargins(marginAmount/10, marginAmount/10, marginAmount/10, marginAmount/10);		
		} else{
			tabWidget->widget(n)->setContentsMargins(0, 0, 0, 0);		
		}
	}
}

uiTab *uiNewTab(void)
{
	auto tabWidget = new QTabWidget;

	// note styling is being set in main.cpp -> styleSheet

	return uiAllocQt5ControlType(uiTab,tabWidget,uiQt5Control::DeleteControlOnQObjectFree);
}
