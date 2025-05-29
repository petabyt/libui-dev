#include "uipriv_qt5.hpp"

#include <QProgressBar>

struct uiProgressBar : public uiQt5Control {};

void uiProgressBarSetValue(uiProgressBar *p, int value)
{
	if (auto progressBar = uiValidateAndCastObjTo<QProgressBar>(p)) {
		if(value == -1){
			progressBar->setMaximum(0);
			progressBar->setMinimum(0);
		} else {
			progressBar->setMinimum(0);
			progressBar->setMaximum(100);
			progressBar->setValue(value);
		}
	}
}

int uiProgressBarValue(uiProgressBar *p)
{
	if (auto progressBar = uiValidateAndCastObjTo<QProgressBar>(p)) {
		return progressBar->value();
	}
	return -1;
}

uiProgressBar *uiNewProgressBar(void)
{
	auto progressBar = new QProgressBar;
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);

	// note styling is being set in main.cpp -> styleSheet

	return uiAllocQt5ControlType(uiProgressBar,progressBar,uiQt5Control::DeleteControlOnQObjectFree);
}
