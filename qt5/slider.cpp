#include "uipriv_qt5.hpp"

#include <QSlider>

struct uiSlider : public uiQt5Control {};

int uiSliderValue(uiSlider *s)
{
	if (auto slider = uiValidateAndCastObjTo<QSlider>(s)) {
		return slider->value();
	}
	return 0;
}

void uiSliderSetValue(uiSlider *s, int value)
{
	if (auto slider = uiValidateAndCastObjTo<QSlider>(s)) {
		return slider->setValue((int)value);
	}
}

void uiSliderOnChanged(uiSlider *s, void (*f)(uiSlider *, void *), void *data)
{
	if (auto slider = uiValidateAndCastObjTo<QSlider>(s)) {
		QObject::connect(slider, &QSlider::valueChanged, slider, [f,s,data]{
			f(s,data);
		}, Qt::UniqueConnection);
	}
}

uiSlider *uiNewSlider(int min, int max)
{
	auto slider = new QSlider(Qt::Horizontal);
	slider->setRange(qMin<int>(min,max), qMax<int>(min,max));

	// note styling is being set in main.cpp -> styleSheet

	return uiAllocQt5ControlType(uiSlider,slider,uiQt5Control::DeleteControlOnQObjectFree);
}
