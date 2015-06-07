#include <QFileDialog>
#include "SourcePackSetting.h"
#include "Model/PathManager.h"
#include "Dialogs/BjMessageBox.h"

SourcePackSetting::SourcePackSetting(QWidget *parent) :
QDialog(parent),
ui(new Ui::SourcePackSetting)
{
	ui->setupUi(this);
	InitView();
	InitSlot();
}

SourcePackSetting::~SourcePackSetting()
{
	delete ui;
}

void SourcePackSetting::InitView()
{
	ui->LineEditPath->setText(PathManager::GetSrcPath());
	ui->LineEditSdkPath->setText(PathManager::GetSdkPath());
	ui->ComboBoxEncode->addItem(QStringLiteral("UTF-8"));
	ui->ComboBoxEncode->addItem(QStringLiteral("GBK"));
	ui->ComboBoxEncode->setCurrentIndex(PathManager::GetEncode());
}

void SourcePackSetting::InitSlot()
{
	connect(ui->ButtonScan, SIGNAL(clicked()), this, SLOT(ButtonScanClickSlot()));
	connect(ui->ButtonOk, SIGNAL(clicked()), this, SLOT(ButtonOkClickSlot()));
	connect(ui->ButtonScanSdk, SIGNAL(clicked()), this, SLOT(ButtonScanSdkClickSlot()));
}

void SourcePackSetting::ButtonScanClickSlot()
{
	QString defaultPath = PathManager::ReadLastPath(QStringLiteral("SrcPath"));
	if (defaultPath.isEmpty()){
		defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	}
	QString	dir = QFileDialog::getExistingDirectory(this, QStringLiteral("ѡ��androidԴ��·��"), defaultPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()){
		ui->LineEditPath->setText(dir);
	}
}

void SourcePackSetting::ButtonScanSdkClickSlot()
{
	QString defaultPath = PathManager::ReadLastPath(QStringLiteral("SdkPath"));
	if (defaultPath.isEmpty()){
		defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	}
	QString	dir = QFileDialog::getExistingDirectory(this, QStringLiteral("ѡ��AndroidSdk·��"), defaultPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()){
		ui->LineEditSdkPath->setText(dir);
	}
}

void SourcePackSetting::ButtonOkClickSlot()
{
	if (ui->LineEditPath->text().isEmpty() || ui->LineEditSdkPath->text().isEmpty()){
		BjMessageBox::warning(this, QStringLiteral("������ʾ"), QStringLiteral("��Դ��·����sdk·��ȫ������"));
		return;
	}
	PathManager::SetSrcPath(ui->LineEditPath->text());
	PathManager::SetSdkPath(ui->LineEditSdkPath->text());
	PathManager::SetEncode(ui->ComboBoxEncode->currentIndex());
	PathManager::WriteLastPath(QStringLiteral("SrcPath"), ui->LineEditPath->text());
	PathManager::WriteLastPath(QStringLiteral("SdkPath"), ui->LineEditSdkPath->text());
}