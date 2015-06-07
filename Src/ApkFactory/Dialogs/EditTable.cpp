#include <QSqlError>
#include "EditTable.h"
#include "Model/DatabaseManager.h"
#include "Dialogs/BjMessageBox.h"
#include "Model/BjTableModel.h"

EditTable::EditTable(QWidget *parent, const QModelIndex *index) :
QDialog(parent),
ui(new Ui::EditTable),
madapterStr(),
madapterRes(),
madapterPak(),
madapterAppPak(),
mpindex(index)
{
	ui->setupUi(this);
	InitModel(index);
	InitView();
	InitSlot();
}

EditTable::~EditTable()
{
	delete ui;
}

void EditTable::InitModel(const QModelIndex *index)
{
	if (index->column() == 3){
		ui->TableView->setModel(DatabaseManager::GetInstance()->GetTableModelStr(*index));
		ui->TableView->SetAapter(&madapterStr);
	}
	else if (index->column() == 4){
		ui->TableView->setModel(DatabaseManager::GetInstance()->GetTableModelRes(*index));
		ui->TableView->SetAapter(&madapterRes);
	}
	else if (index->column() == 5){
		ui->TableView->setModel(DatabaseManager::GetInstance()->GetTableModelPak(*index));
		ui->TableView->SetAapter(&madapterPak);
	}
	else if (index->column() == 6){
		ui->TableView->setModel(DatabaseManager::GetInstance()->GetTableModelAppPak(*index));
		ui->TableView->SetAapter(&madapterAppPak);
	}
	ui->TableView->hideColumn(0);
	ui->TableView->hideColumn(1);
}

void EditTable::InitSlot()
{
	connect(ui->ButtonNewItem, SIGNAL(clicked()), this, SLOT(NewItem()));
	connect(ui->ButtonDelItem, SIGNAL(clicked()), this, SLOT(DelItem()));
	connect(ui->ButtonOk, SIGNAL(clicked()), this, SLOT(OkClick()));
}

void EditTable::InitView()
{
	ui->TableView->setSortingEnabled(true);
}

void EditTable::NewItem()
{
	if (mpindex->column() == 3){
		madapterStr.AddRow();
	}
	else if (mpindex->column() == 4){
		madapterRes.AddRow();
	}
	else if (mpindex->column() == 5){
		madapterPak.AddRow();
	}
	else if (mpindex->column() == 6){
		madapterAppPak.AddRow();
	}
}

void EditTable::DelItem()
{
	if (mpindex->column() == 3){
		madapterStr.DelRow(ui->TableView->GetSelectIndexs());
	}
	else if (mpindex->column() == 4){
		madapterRes.DelRow(ui->TableView->GetSelectIndexs());
	}
	else if (mpindex->column() == 5){
		madapterPak.DelRow(ui->TableView->GetSelectIndexs());
	}
	else if (mpindex->column() == 6){
		madapterAppPak.DelRow(ui->TableView->GetSelectIndexs());
	}
}

void EditTable::OkClick()
{
	BjTableModel *tbModel = (BjTableModel *)(ui->TableView->model());
	if (!tbModel->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(tbModel->lastError().text()));
		tbModel->revertAll();//�����ɾ��������
	}
}