// 8 june 2016
#include "uipriv_qt5.hpp"
#include <QGridLayout>
#include <QWidget>

struct uiGrid : public uiQt5Control {};


void uiGridAppend(uiGrid *g, uiControl *c, int left, int top, int xspan, int yspan, int hexpand, uiAlign halign, int vexpand, uiAlign valign)
{
	qWarning("TODO hexpand vexpand");

	if (auto layoutGrid = uiValidateAndCastObjTo<QGridLayout>(g)) {
		auto obj = uiValidateAndCastObjTo<QObject>(c);

		Qt::Alignment alignH;
		Qt::Alignment alignV;
		switch(halign){
			case uiAlignFill:
				break;
			case uiAlignStart:
				alignH = Qt::AlignLeft;
				break;
			case uiAlignCenter:
				alignH = Qt::AlignHCenter;
				break;
			case uiAlignEnd:
				alignH = Qt::AlignRight;
				break;
		}

		switch(valign){
			case uiAlignFill:
				break;
			case uiAlignStart:
				alignV = Qt::AlignTop;
				break;
			case uiAlignCenter:
				alignV = Qt::AlignVCenter;
				break;
			case uiAlignEnd:
				alignV = Qt::AlignBottom;
				break;
		}

		if(vexpand){
			yspan = -1;
		}
		if(hexpand){
			xspan = -1;
		}

		if (auto layout = qobject_cast<QLayout*>(obj)) {
			layoutGrid->addLayout(layout, top, left, yspan, xspan, alignH | alignV);
		} else if (auto widget = qobject_cast<QWidget*>(obj)) {
			layoutGrid->addWidget(widget, top, left, yspan, xspan, alignH | alignV);
		} else {
			qWarning("object is neither layout nor widget");
		}
	}
}

void uiGridInsertAt(uiGrid *g, uiControl *c, uiControl *existing, uiAt at, int xspan, int yspan, int hexpand, uiAlign halign, int vexpand, uiAlign valign)
{
	qWarning("TODO uiGridInsertAt");
	// struct gridChild gc;
	// GtkWidget *widget;

	// widget = prepare(&gc, c, hexpand, halign, vexpand, valign);
	// uiControlSetParent(gc.c, uiControl(g));
	// TODO_MASSIVE_HACK(uiUnixControl(gc.c));
	// gtk_grid_attach_next_to(g->grid, widget,
	// 	GTK_WIDGET(uiControlHandle(existing)), gtkPositions[at],
	// 	xspan, yspan);
	// g_array_append_val(g->children, gc);
}

int uiGridPadded(uiGrid *g)
{
	if (auto layoutGrid = uiValidateAndCastObjTo<QGridLayout>(g)) {
		return layoutGrid->horizontalSpacing() > 0;
	}
	return 0;
}

void uiGridSetPadded(uiGrid *g, int padded)
{
	if (auto layoutGrid = uiValidateAndCastObjTo<QGridLayout>(g)) {
		if(padded){
			layoutGrid->setHorizontalSpacing(10);
			layoutGrid->setVerticalSpacing(10);
		} else {
			layoutGrid->setHorizontalSpacing(0);
			layoutGrid->setVerticalSpacing(0);
		}
	}
}

uiGrid *uiNewGrid(void)
{
	QGridLayout *layout = new QGridLayout();
	layout->setHorizontalSpacing(0);
	layout->setVerticalSpacing(0);

	return uiAllocQt5ControlType(uiGrid, layout, uiQt5Control::DeleteControlOnQObjectFree);
}
