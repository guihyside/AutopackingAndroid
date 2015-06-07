#include <QFileDialog>
#include <QStandardPaths>
#include "SetKey.h"
#include "Model/PathManager.h"

SetKey::SetKey(QWidget *parent) :
QDialog(parent),
ui(new Ui::SetKey)
{
	ui->setupUi(this);
	InitView();
	InitSlot();	
}
SetKey::~SetKey()
{
	delete ui;
}

void SetKey::InitView()
{
	QRegExp rx("^[\\@A-Za-z0-9\\!\\#\\$\\%\\^\\&\\*\\.\\~]{6,200}$");
	QValidator *validator = new QRegExpValidator(rx, this);
	ui->LineEditKey->setValidator(validator);
	ui->LineEditAliasesPasswd->setValidator(validator);
	mkeyPath = PathManager::GetKeyPath();
	ui->LineEditPath->setText(mkeyPath);
	ui->LineEditKey->setText(PathManager::GetPasswd());
	ui->LineEditAliases->setText(PathManager::GetKeyAliases());
	ui->LineEditAliasesPasswd->setText(PathManager::GetAliasesPasswd());
	ui->ComboBoxSigalg->insertItem(0, QString("MD5withRSA"));
	ui->ComboBoxSigalg->insertItem(1, QString("SHA1withDSA"));
	ui->ComboBoxDigestalg->insertItem(0, QString("SHA1"));
	ui->ComboBoxSigalg->setEditText(PathManager::GetSigalg());
	ui->ComboBoxDigestalg->setEditText(PathManager::GetDigestalg());
}

void SetKey::InitSlot()
{
	connect(ui->RadioButtonKey, SIGNAL(clicked()), this, SLOT(ChangStatusKeySlot()));
	connect(ui->RadioButtonAliaKey, SIGNAL(clicked()), this, SLOT(ChangStatusAlisKeySlot()));
	connect(ui->ButtonScan, SIGNAL(clicked()), this, SLOT(ButtonScanClickSlot()));
	connect(ui->ButtonOk, SIGNAL(clicked()), this, SLOT(ButtonOkClickSlot()));
}

void SetKey::ChangStatusKeySlot()
{
	if (ui->RadioButtonKey->isChecked()){ 
		ui->LineEditKey->setEchoMode(QLineEdit::Normal);
	}
	else{ 
		ui->LineEditKey->setEchoMode(QLineEdit::Password);
	}
}

void SetKey::ChangStatusAlisKeySlot()
{
	if (ui->RadioButtonAliaKey->isChecked()){
		ui->LineEditAliasesPasswd->setEchoMode(QLineEdit::Normal);
	}
	else{
		ui->LineEditAliasesPasswd->setEchoMode(QLineEdit::Password);
	}
}

void SetKey::ButtonScanClickSlot()
{
	QString defaultPath = PathManager::ReadLastPath(QStringLiteral("setkey"));
	if (defaultPath.isEmpty()){
		defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	}
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("���ļ�"), defaultPath, QStringLiteral("key file (*.keystore)"));
	if (!fileName.isEmpty()){
		mkeyPath = fileName;
		ui->LineEditPath->setText(fileName);
	}
}

void SetKey::ButtonOkClickSlot()
{
	PathManager::SetKeyPath(mkeyPath);
	PathManager::WriteLastPath(QStringLiteral("setkey"), mkeyPath.left(mkeyPath.replace("\\","/").lastIndexOf("/")));
	QString passwrd = ui->LineEditKey->text();
	PathManager::SetPasswd(passwrd);
	QString aliases = ui->LineEditAliases->text();
	PathManager::SetKeyAliases(aliases);
	QString aliasesPasswd = ui->LineEditAliasesPasswd->text();
	PathManager::SetAliasesPasswd(aliasesPasswd);
	QString sigal = ui->ComboBoxSigalg->currentText();
	PathManager::SetSigalg(sigal);
	QString digestalg = ui->ComboBoxDigestalg->currentText();
	PathManager::SetDigestalg(digestalg);
}