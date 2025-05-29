// 8 june 2016
#include "uipriv_qt5.hpp"
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>

struct uiForm : public uiQt5Control {};

void uiFormAppend(uiForm *f, const char *label, uiControl *c, int stretchy)
{
	if (auto layoutForm = uiValidateAndCastObjTo<QFormLayout>(f)) {
		auto obj = uiValidateAndCastObjTo<QObject>(c);

		if (auto layout = qobject_cast<QLayout*>(obj)) {
			layoutForm->addRow(new QLabel(QString::fromUtf8(label)), layout);
		} else if (auto widget = qobject_cast<QWidget*>(obj)) {
			QSizePolicy sp;
			if(stretchy){
				widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
				sp = widget->sizePolicy();
				sp.setVerticalStretch(1);
			} else {
				widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
				sp = widget->sizePolicy();
			}
			sp.setHorizontalStretch(1);
			widget->setSizePolicy(sp);
			layoutForm->addRow(new QLabel(QString::fromUtf8(label)), widget);
		} else {
			qWarning("object is neither layout nor widget");
		}
	}
}

void uiFormDelete(uiForm *f, int index)
{
	if (auto layoutForm = uiValidateAndCastObjTo<QFormLayout>(f)) {
		layoutForm->removeRow(index);
	}
}

int uiFormPadded(uiForm *f)
{
	if (auto layoutForm = uiValidateAndCastObjTo<QFormLayout>(f)) {
		return layoutForm->spacing() > 0;
	}
	return 0;
}

void uiFormSetPadded(uiForm *f, int padded)
{
	if (auto layoutForm = uiValidateAndCastObjTo<QFormLayout>(f)) {
		if(padded) {
			layoutForm->setSpacing(10);
			layoutForm->setVerticalSpacing(10);
		} else {
			layoutForm->setSpacing(0);
			layoutForm->setVerticalSpacing(0);			
		}
	}
}

uiForm *uiNewForm(void)
{
    QFormLayout *layout = new QFormLayout();
    layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    return uiAllocQt5ControlType(uiForm, layout, uiQt5Control::DeleteControlOnQObjectFree);
}
