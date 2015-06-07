#include <QFileDialog>
#include "ImportData.h"
#include "Model/DatabaseManager.h"
#include "Model/PathManager.h"
#include "Dialogs/BjMessageBox.h"

ImportData::ImportData(QWidget *parent):
QDialog(parent),
ui(new Ui::ImportData)
{
	ui->setupUi(this);
	InitSlot();
}
ImportData::~ImportData()
{
	delete ui;
}

void ImportData::InitSlot()
{
	connect(ui->ButtonScan, SIGNAL(clicked()), this, SLOT(ButtonScanClickSlot()));
	connect(ui->ButtonOk, SIGNAL(clicked()), this, SLOT(ButtonOkClickSlot()));
}

void ImportData::ButtonScanClickSlot()
{
	QString defaultPath = PathManager::ReadLastPath(QStringLiteral("importData"));
	if (defaultPath.isEmpty()){
		defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	}
	QString	fileName = QFileDialog::getOpenFileName(this, QStringLiteral("ָ�������ļ�"), defaultPath, QStringLiteral("*.db"));
	if (!fileName.isEmpty()){
		if (!fileName.endsWith(".db")){
			fileName.append(".db");
		}
		mimportDataFile = fileName;
		ui->LineEditPath->setText(fileName);
		PathManager::WriteLastPath(QStringLiteral("importData"), fileName.left(fileName.replace("\\", "/").lastIndexOf("/")));
	}
}

void ImportData::ButtonOkClickSlot()
{
	QFile imFile(mimportDataFile);
	if (mimportDataFile.isEmpty() || !imFile.exists()){
		BjMessageBox::warning(NULL, QStringLiteral("�������"), QStringLiteral("������ļ������ڣ���ѡ�����ļ�"), QMessageBox::Ok);
		return;
	}
	DatabaseManager::GetInstance()->ImportData(mimportDataFile);
}