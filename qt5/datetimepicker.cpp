#include "uipriv_qt5.hpp"

//#include <QCalendarWidget> // TODO: for dropdown
#include <QDate>
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>

struct uiDateTimePicker : public uiQt5Control {};

void dateTimeTotm(QDateTime *datetime, struct tm *t){
	QDate date = datetime->date();
	QTime time = datetime->time();
	t->tm_year = date.year() - 1900;
	t->tm_mon = date.month();
	t->tm_mday = date.day();
	t->tm_hour = time.hour();
	t->tm_min = time.minute();
	t->tm_sec = time.second();
}

void uiDateTimePickerSetTime(uiDateTimePicker *d, const struct tm *time)
{
	if (auto control = uiValidateAndCastObjTo<QDateTimeEdit>(d)) {
		QDateTime date = QDateTime(QDate(time->tm_year, time->tm_mon, time->tm_mday),
								   QTime(time->tm_hour, time->tm_min, time->tm_sec));
		control->setDateTime(date);
	}
}

void uiDateTimePickerOnChanged(uiDateTimePicker *d, void (*f)(uiDateTimePicker *, void *), void *data)
{
	if (auto control = uiValidateAndCastObjTo<QDateTimeEdit>(d)) {
		QObject::connect(control, &QDateTimeEdit::dateTimeChanged, control, [f,d,data]{
			f(d, data);
		}, Qt::UniqueConnection);
	}
}

void uiDateTimePickerTime(uiDateTimePicker *d, struct tm *t)
{
	if (auto control = uiValidateAndCastObjTo<QDateTimeEdit>(d)) {
		QDateTime datetime = control->dateTime();
		dateTimeTotm(&datetime, t);
	}
}


uiDateTimePicker *uiNewDateTimePicker(void)
{
	auto dateEdit = new QDateTimeEdit(QDateTime::currentDateTime());

	// note styling is being set in main.cpp -> styleSheet

	return uiAllocQt5ControlType(uiDateTimePicker,dateEdit,uiQt5Control::DeleteControlOnQObjectFree);
}

uiDateTimePicker *uiNewDatePicker(void)
{
	auto timeEdit = new QDateEdit(QDate::currentDate());

	// note styling is being set in main.cpp -> styleSheet

	return uiAllocQt5ControlType(uiDateTimePicker,timeEdit,uiQt5Control::DeleteControlOnQObjectFree);
}

uiDateTimePicker *uiNewTimePicker(void)
{
	auto dateTimeEdit = new QTimeEdit(QTime::currentTime());

	// note styling is being set in main.cpp -> styleSheet

	return uiAllocQt5ControlType(uiDateTimePicker,dateTimeEdit,uiQt5Control::DeleteControlOnQObjectFree);
}
