#include <QAction>
#include <QPushButton>
#include "BjTableView.h"

BjTableView::BjTableView(QWidget *parent):
QTableView(parent),
mmenu(parent),
mpinterface(NULL)
{
	InitView();
	InitSlot();	
}

BjTableView::~BjTableView()
{
	pactionNew->deleteLater();
	pactionDel->deleteLater();
	for (QSet<QAction *>::iterator ite = mactionSet.begin(); ite != mactionSet.end(); ite++)
	{
		(*ite)->deleteLater();
	}
}

void BjTableView::InitSlot()
{
	connect(pactionNew, SIGNAL(triggered(bool)), SLOT(ActionNewSlot()));
	connect(pactionDel, SIGNAL(triggered(bool)), SLOT(ActionDelSlot()));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(DoubleClick(const QModelIndex &)));
}

void BjTableView::InitView()
{
	pactionNew = new QAction(this);
	pactionDel = new QAction(this);
	pactionNew->setText(QStringLiteral("�½���"));
	pactionDel->setText(QStringLiteral("ɾ����"));
}

void BjTableView::contextMenuEvent(QContextMenuEvent *event)
{
	mmenu.clear(); //���ԭ�в˵�
	mmenu.addAction(pactionNew);
	mmenu.addAction(pactionDel);
	for (QSet<QAction *>::iterator ite = mactionSet.begin(); ite != mactionSet.end(); ite++)
	{
		mmenu.addAction(*ite);
	}
	mmenu.exec(QCursor::pos());
}

void BjTableView::SetAapter(ITableContral *pinterface)
{
	mpinterface = pinterface;
}

void BjTableView::AddAction(QAction *action)
{
	mactionSet.insert(action);
}

void BjTableView::ActionNewSlot()
{
	if (mpinterface != NULL){ 
		mpinterface->AddRow();
	}
}

void BjTableView::ActionDelSlot()
{
	if (mpinterface != NULL){
		mpinterface->DelRow(this->selectedIndexes());
	}
}

void BjTableView::DoubleClick(const QModelIndex &index)
{
	if (mpinterface != NULL){
		mpinterface->DoubleClick(index);
	}
}

QModelIndexList BjTableView::GetSelectIndexs()
{
	return this->selectedIndexes();
}