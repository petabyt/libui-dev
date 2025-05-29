#include "uipriv_qt5.hpp"

#include <QMainWindow>

#include <QCloseEvent>
#include <QMenuBar>
#include <QLayout>

#include <functional>

class WindowWidget : public QMainWindow
{
public:
	std::function<void (QCloseEvent *event)> onClosing;
	void closeEvent(QCloseEvent *event)
	{
		if (onClosing) {
			onClosing(event);
		}
	}

	std::function<void (QResizeEvent *event)> onResize;
	void resizeEvent(QResizeEvent * event)
	{
		if(onResize){
			onResize(event);
		}
	}
};

char *uiWindowTitle(uiWindow *w)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		return uiQt5StrdupQString(window->windowTitle());
	}
	return nullptr;
}

void uiWindowSetTitle(uiWindow *w, const char *title)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		window->setWindowTitle(QString::fromUtf8(title));
	}
}

void uiWindowOnClosing(uiWindow *w, int (*f)(uiWindow *, void *), void *data)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		if (f) {
			window->onClosing = [f, w, data](QCloseEvent *event){
				if (f(w,data)) {
					// eat the event and destroy the control
					// normally such behavior would be achived with
					// setAttribute(Qt::WA_DeleteOnClose, true)
					// but we need to behave consistently
					event->accept();

					// TODO: segfaults:
					// w->Destroy(w);
				}
			};
		} else {
			// clear callback
			window->onClosing = nullptr;
		}
	}
}

void uiWindowSetChild(uiWindow *w, uiControl *child)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		auto obj = uiValidateAndCastObjTo<QObject>(child);
		if (window->centralWidget()) {
			window->centralWidget()->deleteLater();
		}
		if (auto layout = qobject_cast<QLayout*>(obj)) {
			auto widget = new QWidget;
			widget->setLayout(layout);
			window->setCentralWidget(widget);
		} else if (auto widget = qobject_cast<QWidget*>(obj)) {
			window->setCentralWidget(widget);
		} else {
			qWarning("object is neither layout nor widget");
		}
	}
}

int uiWindowFullscreen(uiWindow *w)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		Qt::WindowStates state = window->windowState();
		return state & Qt::WindowFullScreen;
	}
	return -1;
}

void uiWindowSetFullscreen(uiWindow *w, int fullscreen)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		window->showFullScreen();
	}
}

int uiWindowMargined(uiWindow *w)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		QMargins s = window->contentsMargins();
		return s.top() == marginAmount;
	}
	return 0;
}

void uiWindowSetMargined(uiWindow *w, int margined)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		if(margined){
			window->setContentsMargins(marginAmount, marginAmount, marginAmount, marginAmount);
		} else {
			window->setContentsMargins(0, 0, 0, 0);
		}
	}
}

void uiWindowSetBorderless(uiWindow *w, int borderless)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		if(borderless){
			window->setWindowFlags(window->windowFlags() | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);		
		} else {
			window->setWindowFlags(window->windowFlags() & ~Qt::CustomizeWindowHint & Qt::FramelessWindowHint);		
		}
	}
}


void uiWindowContentSize(uiWindow *w, int *width, int *height)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		QSize size = window->size();
		*width = size.width();
		*height = size.height();
	}
}

void uiWindowSetContentSize(uiWindow *w, int width, int height)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		window->resize(width, height);
	}
}

void uiWindowOnContentSizeChanged(uiWindow *w, void (*f)(uiWindow *, void *), void *data)
{
	if (auto window = uiValidateAndCastObjTo<WindowWidget>(w)) {
		if (f) {
			window->onResize = [f, w, data](QResizeEvent *event){
				f(w, data);
				event->accept();
			};
		} else {
			// clear callback
			window->onResize = nullptr;
		}
	}
}

uiWindow *uiNewWindow(const char *title, int width, int height, int hasMenubar)
{
	auto window = new WindowWidget;

	window->setWindowTitle(title);
	window->resize(width,height);

	if (hasMenubar) {
		window->setMenuBar(uiQt5FindMainMenuBar());
	}

	return uiAllocQt5ControlType(uiWindow, window, uiQt5Control::DeleteControlOnQObjectFree);
}
