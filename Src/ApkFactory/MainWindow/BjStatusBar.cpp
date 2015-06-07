#include "BjStatusBar.h"

BjStatusBar::BjStatusBar(QWidget *parent)
:QStatusBar(parent),
mlableTime(parent),
mtext1(parent)
{
	InitView();
}

void BjStatusBar::InitView()
{
	mlableTime.setAlignment(Qt::AlignCenter);
	mlableTime.setMinimumSize(mlableTime.sizeHint());
	this->setSizeGripEnabled(false); //�����Ƿ���ʾ�ұߵĴ�С���Ƶ�
	this->addWidget(&mlableTime);
	this->addPermanentWidget(&mlableTime); //��ʾ������Ϣ

	mtext1.setAlignment(Qt::AlignCenter);
	mtext1.setMinimumSize(mtext1.sizeHint());
	this->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // ���ò���ʾlabel�ı߿�
	this->addWidget(&mtext1);
}

void BjStatusBar::SetText1(QString &text)
{
	mtext1.setText(text);
}

void BjStatusBar::ShowTime(QString time)
{
	mlableTime.setText(time);
}