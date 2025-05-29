#include "uipriv_qt5.hpp"

#include <QRadioButton>
#include <QVBoxLayout>

struct uiRadioButtons : public uiQt5Control {};

struct RadioLayout : public QVBoxLayout {
	std::function<void()> onSelect;
};


int uiRadioButtonsSelected(uiRadioButtons *r)
{
	if (auto layout = uiValidateAndCastObjTo<RadioLayout>(r)) {
		for(int i = 0; i < layout->count(); i++){
			if(((QRadioButton*) layout->itemAt(i)->widget())->isChecked()){
				return i;
			}
		}
	}
	return -1;
}

void uiRadioButtonsSetSelected(uiRadioButtons *r, int n)
{
	if (auto layout = uiValidateAndCastObjTo<RadioLayout>(r)) {
		for(int i = 0; i < layout->count(); i++){
			QRadioButton *btn = (QRadioButton*) layout->itemAt(i)->widget();
			btn->setChecked(i == n);
		}
	}
}

void uiRadioButtonsOnSelected(uiRadioButtons *r, void (*f)(uiRadioButtons *, void *), void *data)
{
	if (auto layout = uiValidateAndCastObjTo<RadioLayout>(r)) {
		if(f == nullptr){
			layout->onSelect = nullptr;
		} else {
			layout->onSelect = [f, r, data](){
				f(r, data);
			};
		}
	}
}

void uiRadioButtonsAppend(uiRadioButtons *r, const char *text)
{
	if (auto layout = uiValidateAndCastObjTo<RadioLayout>(r)) {
		QRadioButton *radioButton = new QRadioButton(QString::fromUtf8(text));
		layout->addWidget(radioButton);
		if (layout->count() == 1) {
			radioButton->setChecked(true);
		}
		QObject::connect(radioButton, &QRadioButton::toggled, [layout](bool checked){
			if(checked && layout->onSelect) {
				layout->onSelect();
			}
		});
	}
}

uiRadioButtons *uiNewRadioButtons(void)
{
	// TODO: check does this need a QButtonGroup or is the layout sufficent?
	auto layout = new RadioLayout();
	layout->setSpacing(5);

	// note styling is being set in main.cpp -> styleSheet

	return uiAllocQt5ControlType(uiRadioButtons,layout,uiQt5Control::DeleteControlOnQObjectFree);
}
