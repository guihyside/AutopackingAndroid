#include <QFileDialog>
#include "ExportData.h"
#include "Model/PathManager.h"
#include "Model/DatabaseManager.h"
#include "Dialogs/BjMessageBox.h"

ExportData::ExportData(QWidget *parent) :
QDialog(parent),
ui(new Ui::ExportData)
{
	ui->setupUi(this);
	InitSlot();
}
ExportData::~ExportData()
{
	delete ui;
}

void ExportData::InitSlot()
{
	connect(ui->ButtonOk, SIGNAL(clicked()), this, SLOT(ButtonOkClickSlot()));
	connect(ui->ButtonScan, SIGNAL(clicked()), this, SLOT(ButtonScanClickSlot()));
}

void ExportData::ButtonOkClickSlot()
{
	if (mexportDataFile.isEmpty()){
		BjMessageBox::warning(NULL, QStringLiteral("��������"), QStringLiteral("�����ļ�δ���ã��������õ����ļ�"), QMessageBox::Ok);
		return;
	}
	DatabaseManager::GetInstance()->ExportData(mexportDataFile);
}

void ExportData::ButtonScanClickSlot()
{
	QString defaultPath = PathManager::ReadLastPath(QStringLiteral("exportData"));
	if (defaultPath.isEmpty()){
		defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	}
	QString	fileName = QFileDialog::getSaveFileName(this, QStringLiteral("ָ�������ļ�"), defaultPath, QStringLiteral("*.db"));
	if (!fileName.isEmpty()){
		if (!fileName.endsWith(".db")){
			fileName.append(".db");
		}
		mexportDataFile = fileName;
		ui->LineEditPath->setText(fileName);
		PathManager::WriteLastPath(QStringLiteral("exportData"), fileName.left(fileName.replace("\\","/").lastIndexOf("/")));
	}
}