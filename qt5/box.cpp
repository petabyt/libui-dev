#include "uipriv_qt5.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLayoutItem>
#include <QWidget>

struct uiBox : public uiQt5Control {};

void uiBoxAppend(uiBox *b, uiControl *c, int stretchy)
{
	if (auto layoutBox = uiValidateAndCastObjTo<QBoxLayout>(b)) {
		auto obj = uiValidateAndCastObjTo<QObject>(c);

		if (auto layout = qobject_cast<QLayout*>(obj)) {
			layoutBox->addLayout(layout, stretchy);
		} else if (auto widget = qobject_cast<QWidget*>(obj)) {
			if(stretchy){
				if(layoutBox->alignment() == Qt::AlignTop){
					widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
				} else {
					widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
				}
			} else {
				if(layoutBox->alignment() == Qt::AlignTop){
					widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
				} else {
					widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
				}
			}
			layoutBox->addWidget(widget);
		} else {
			qWarning("object is neither layout nor widget");
		}
	}
}

void uiBoxDelete(uiBox *b, int index)
{
	if (auto layoutBox = uiValidateAndCastObjTo<QBoxLayout>(b)) {
		if (index < layoutBox->count()) {
			delete layoutBox->takeAt(index);
		}
	}
}

int uiBoxPadded(uiBox *b)
{
	if (auto layoutBox = uiValidateAndCastObjTo<QBoxLayout>(b)) {
		return layoutBox->spacing() > 0;
	}
	return 0;
}

void uiBoxSetPadded(uiBox *b, int padded)
{
	if (auto layoutBox = uiValidateAndCastObjTo<QBoxLayout>(b)) {
		if(padded){
			layoutBox->setSpacing(10);
		} else {
			layoutBox->setSpacing(0);
		}
	}
}

uiBox *uiNewHorizontalBox(void)
{
	auto layout = new QHBoxLayout();
	layout->setAlignment(Qt::AlignLeft);

	return uiAllocQt5ControlType(uiBox, layout, uiQt5Control::DeleteControlOnQObjectFree);
}

uiBox *uiNewVerticalBox(void)
{
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);

	return uiAllocQt5ControlType(uiBox, layout, uiQt5Control::DeleteControlOnQObjectFree);
}
