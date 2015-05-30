#include <QTableView>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QProcess>
#include <QSet>
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkReply>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Model/DatabaseManager.h"
#include "Model/PathManager.h"
#include "Dialogs/About.h"
#include "Dialogs/Donate.h"
#include "Dialogs/BjMessageBox.h"
#include "Dialogs/ImportData.h"
#include "Dialogs/ExportData.h"
#include "Dialogs/SetJdkPath.h"
#include "Dialogs/SetKey.h"
#include "Dialogs/SourcePackSetting.h"
#include "Dialogs/DecPackSetting.h"
#include "Dialogs/ThreadConfigDialog.h"
#include "Dialogs/SetVersionDialog.h"
#include "Dialogs/DePack.h"
#include "Works/DecPack.h"
#include "Works/SrcPack.h"


MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow),
mtoolBar(parent),
mcentralFram(parent),
mstatusBar(parent),
mthreadNum(PathManager::GetThreadNum()),
mversion(PathManager::GetVersion()),
mcurrentTaskIndex(0),
mplogDialog(NULL),
misLogShowing(false),
mnetworkManager(new QNetworkAccessManager(parent))
{
    ui->setupUi(this);
	InitView();
	InitData();
	InitSlot();
	//QUrl url(WEB_SITE);
	//mnetworkManager->get(QNetworkRequest(url));
}

MainWindow::~MainWindow()
{
    delete ui;
	DatabaseManager::GetInstance()->Destroyed();
}

void MainWindow::InitData()
{
	mcentralFram.GetTableView().SetAapter(&madapter);
	StatusTextChang();
	PathManager::CheckSysEnvironment();
	if (PathManager::IsFirstRun()){
		BjMessageBox::information(this, QStringLiteral("������ʾ"), QStringLiteral("��⵽���ǵ�һ��ʹ�ñ���������������Ķ�ʹ��˵����ʹ�ã����������ɲ���Ҫ�Ĵ������ʧ��"), QMessageBox::Ok, QMessageBox::NoButton);
		HeldSlot();
	}
}

void MainWindow::InitView()
{
	this->addToolBar((QToolBar *)&mtoolBar);
	this->setCentralWidget(&mcentralFram);
	this->setStatusBar(&mstatusBar);
}

void MainWindow::InitSlot()
{
	connect(ui->actionVersion, SIGNAL(triggered()), this, SLOT(VersionSlot()));
	connect(ui->actionDonate, SIGNAL(triggered()), this, SLOT(DonateSlot()));
	connect(ui->actionImportDb, SIGNAL(triggered()), this, SLOT(ImportDataSlot()));
	connect(ui->actionExportDb, SIGNAL(triggered()), this, SLOT(ExportDataSlot()));
	connect(ui->actionSetJdk, SIGNAL(triggered()), this, SLOT(SetJdkPathSlot()));
	connect(ui->actionSetKey, SIGNAL(triggered()), this, SLOT(SetKeySlot()));
	connect(ui->actionSourceSetting, SIGNAL(triggered()), this, SLOT(SourcePackSettingSlot()));
	connect(ui->actionDecPkSetting, SIGNAL(triggered()), this, SLOT(DecPackSettingSlot()));
	connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(HeldSlot()));
	connect(ui->actionUpdata, SIGNAL(triggered()), this, SLOT(UpdataSlot()));
	connect(ui->actionSetVersion, SIGNAL(triggered()), this, SLOT(SetVersionSlot()));
	connect(mtoolBar.GetActionStar(), SIGNAL(triggered()), this, SLOT(StartSlot()));
	connect(mtoolBar.GetActionStop(), SIGNAL(triggered()), this, SLOT(StopSlot()));
	connect(ui->actionDePack, SIGNAL(triggered()), this, SLOT(DePackToolSlot()));
	connect(mtoolBar.GetButtonLog(), SIGNAL(clicked()), this, SLOT(ShowLogSlot()));
	connect(mtoolBar.GetButtonThreadConfig(), SIGNAL(clicked()), this, SLOT(ThreadConfigSlot()));
	connect(mnetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(Httpresponse(QNetworkReply*)));

}

void MainWindow::ChangStat(bool isStar)
{
	mtoolBar.GetActionStar()->setDisabled(isStar);
	mtoolBar.GetActionStop()->setEnabled(isStar);
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString currentDate = currentDateTime.toString("yyyy-MM-dd hh:mm:ss ddd");
	if (isStar){ 
		mstartTime = currentDateTime;
		mstatusBar.ShowTime(QStringLiteral("�����ʼ�� ") + currentDate + QStringLiteral("  "));
		mlog.clear();
		mcentralFram.GetTableView().setEditTriggers(QAbstractItemView::NoEditTriggers);
		mcentralFram.GetTableView().setStyleSheet("background-color:#F7F7F7");
		madapter.SetEnableEdit(false);
		mtoolBar.GetButtonThreadConfig()->setDisabled(true);
		mtoolBar.GetCombox()->setDisabled(true);
		mtoolBar.GetButtonScan()->setDisabled(true);
		ui->menuBar->setDisabled(true);
	}
	else{
		mstatusBar.ShowTime(QStringLiteral("��������� ") + currentDate + QStringLiteral("  "));
		mcentralFram.GetTableView().setEditTriggers(QAbstractItemView::DoubleClicked);
		mcentralFram.GetTableView().setStyleSheet(QString(""));
		madapter.SetEnableEdit(true);
		mtoolBar.GetButtonThreadConfig()->setEnabled(true);
		mtoolBar.GetCombox()->setEnabled(true);
		mtoolBar.GetButtonScan()->setEnabled(true);
		ui->menuBar->setEnabled(true);
	}
}

void MainWindow::StartSlot()
{
	if (!PathManager::CheckSysEnvironment() || !PathManager::CheckParameter()){
		return;
	}
	ChangStat(true);

	if (DatabaseManager::GetInstance()->isDatabaseEmpty()){
		BjMessageBox::warning(NULL, QStringLiteral("����Ϊ��"), QStringLiteral("û���������ݣ������������ݱ�"), QMessageBox::Ok, QMessageBox::NoButton);
		ChangStat(false);
		return;
	}

	QModelIndexList selectedIndex = mcentralFram.GetTableView().GetSelectIndexs();
	QSet<int> dedupli;

	for (QModelIndexList::iterator ite = selectedIndex.begin(); ite != selectedIndex.end(); ite++)
	{
		dedupli.insert(ite->row());
	}
	int rows = DatabaseManager::GetInstance()->GetTableModel()->rowCount();
	if (!dedupli.isEmpty()){
		mrecordIndex = QVector<int>(dedupli.size());
		int i = 0;
		for (QSet<int>::iterator ite = dedupli.begin(); ite != dedupli.end(); ite++)
		{
			mrecordIndex[i++] = *ite;
		}
	}
	else{
		mrecordIndex = QVector<int>(rows);
		for (int i = 0; i < rows; i++)
		{
			mrecordIndex[i] = i;
		}
	}

	for (QVector<int>::iterator ite = mrecordIndex.begin(); ite != mrecordIndex.end(); ite++)
	{
		DatabaseManager::GetInstance()->ChangStatInDatabase(*ite, QString(""));
	}

	mthreadNum = PathManager::GetThreadNum();
	mcurrentTaskIndex = 0;
	switch (mtoolBar.GetCombox()->currentIndex())
	{
	case 0:
		PathManager::CheckBuildXml();
		StartSrcPack();
		break;
	case 1:
		StartDecPack();
		break;
	}
}

void MainWindow::StartDecPack()
{
	if (PathManager::GetDecPackPath().isEmpty()){
		BjMessageBox::warning(NULL, QStringLiteral("��������"), QStringLiteral("�������ģ���δ����!"), QMessageBox::Ok, QMessageBox::NoButton);
		ChangStat(false);
		return;
	}
	else if (!QFile(PathManager::GetDecPackPath()).exists()){
		BjMessageBox::warning(NULL, QStringLiteral("��������"), QStringLiteral("ָ��λ�õķ������ģ���������!"), QMessageBox::Ok, QMessageBox::NoButton);
		ChangStat(false);
		return;
	}
	for (int i = 0; i < mthreadNum; i++,mcurrentTaskIndex++)
	{
		if (mcurrentTaskIndex >= mrecordIndex.size()){
			break;;
		}
		Pack *ppack = new DecPack(this);
		mtaskList.push_back(ppack);
		connect(ppack, SIGNAL(FinishSignal(int,int)), this, SLOT(FinishedSlot(int,int)), Qt::QueuedConnection);
		connect(ppack, SIGNAL(GenerateError(QString)), SLOT(CollectLog(QString)), Qt::QueuedConnection);
		QString id = DatabaseManager::GetInstance()->GetTableModel()->record(mrecordIndex.at(mcurrentTaskIndex)).value("ID").toString();
		QString channelId = DatabaseManager::GetInstance()->GetTableModel()->record(mrecordIndex.at(mcurrentTaskIndex)).value("ChannelID").toString();
		QString channelName = DatabaseManager::GetInstance()->GetTableModel()->record(mrecordIndex.at(mcurrentTaskIndex)).value("ChannelName").toString();
		QList<ReplaceStrTable> strTableList;
		QList<ReplaceResTable> resTableList;
		QList<ReplacePakTable> pakTableList;
		QList<ReplaceAppPakTable> appPakTableList;
		DatabaseManager::GetInstance()->ChangStatInDatabase(mrecordIndex.at(mcurrentTaskIndex), QStringLiteral("�����ʼ��"));
		DatabaseManager::GetInstance()->ReadyData(id, strTableList, resTableList, pakTableList, appPakTableList);
		ppack->Init(PathManager::GetDecPackPath(), PathManager::GetOutPath(), channelId, channelName, id, strTableList, resTableList, pakTableList, appPakTableList,mcurrentTaskIndex);
		ppack->start();
	}

	if (mtaskList.isEmpty()){
		ChangStat(false);
	}
}

void MainWindow::StartSrcPack()
{
	if (PathManager::GetSdkPath().isEmpty()){
		BjMessageBox::warning(NULL, QStringLiteral("��������"), QStringLiteral("Android Sdk·��δ����!"), QMessageBox::Ok, QMessageBox::NoButton);
		ChangStat(false);
		return;
	}
	else if (PathManager::GetSrcPath().isEmpty()){
		BjMessageBox::warning(NULL, QStringLiteral("��������"), QStringLiteral("Դ��·��δ����!"), QMessageBox::Ok, QMessageBox::NoButton);
		ChangStat(false);
		return;
	}

	for (int i = mtaskList.size(); i < mthreadNum; i++, mcurrentTaskIndex++)
	{
		if (mcurrentTaskIndex >= mrecordIndex.size()){
			break;;
		}
		Pack *ppack = new SrcPack(this);
		mtaskList.push_back(ppack);
		connect(ppack, SIGNAL(FinishSignal(int, int)), this, SLOT(FinishedSlot(int, int)), Qt::QueuedConnection);
		connect(ppack, SIGNAL(GenerateError(QString)), SLOT(CollectLog(QString)), Qt::QueuedConnection);
		QString id = DatabaseManager::GetInstance()->GetTableModel()->record(mrecordIndex.at(mcurrentTaskIndex)).value("ID").toString();
		QString channelId = DatabaseManager::GetInstance()->GetTableModel()->record(mrecordIndex.at(mcurrentTaskIndex)).value("ChannelID").toString();
		QString channelName = DatabaseManager::GetInstance()->GetTableModel()->record(mrecordIndex.at(mcurrentTaskIndex)).value("ChannelName").toString();
		QList<ReplaceStrTable> strTableList;
		QList<ReplaceResTable> resTableList;
		QList<ReplacePakTable> pakTableList;
		QList<ReplaceAppPakTable> appPakTableList;
		DatabaseManager::GetInstance()->ChangStatInDatabase(mrecordIndex.at(mcurrentTaskIndex), QStringLiteral("�����ʼ��"));
		DatabaseManager::GetInstance()->ReadyData(id, strTableList, resTableList, pakTableList, appPakTableList);
		ppack->Init(PathManager::GetSrcPath(), PathManager::GetOutPath(), channelId, channelName, id, strTableList, resTableList, pakTableList, appPakTableList,mcurrentTaskIndex);
		ppack->start();
	}

	if (mtaskList.isEmpty()){
		ChangStat(false);
	}
}

void MainWindow::FinishedSlot(int stat,int taskId)
{
	switch (stat)
	{
	case 0:
		for each (Pack * var in mtaskList)
		{
			if (var->GetTaskId() == taskId){
				var->deleteLater();
				mtaskList.removeOne(var);
				DatabaseManager::GetInstance()->ChangStatInDatabase(mrecordIndex.at(taskId), QStringLiteral("������!"));
			}
		}
		break;
	case 1:
		for each (Pack * var in mtaskList)
		{
			if (var->GetTaskId() == taskId){
				var->deleteLater();
				mtaskList.removeOne(var);
				DatabaseManager::GetInstance()->ChangStatInDatabase(mrecordIndex.at(taskId), QStringLiteral("�������!"));
			}
		}
		break;
	case 2:
		for each (Pack * var in mtaskList)
		{
			if (var->GetTaskId() == taskId){
				var->deleteLater();
				mtaskList.removeOne(var);
				DatabaseManager::GetInstance()->ChangStatInDatabase(mrecordIndex.at(taskId), QStringLiteral("���ֹͣ!"));
			}
		}
		return;
	}

	if (mtaskList.size() < mthreadNum){
		switch (mtoolBar.GetCombox()->currentIndex())
		{
		case 0:
			StartSrcPack();
			break;
		case 1:
			StartDecPack();
			break;
		}
		
	}
}

void MainWindow::StopSlot()
{
	ChangStat(false);
	mthreadNum = 0;
	for each (Pack *var in mtaskList)
	{
		var->Stop();
	}
}

void MainWindow::VersionSlot()
{
	About ab(this);
	ab.exec();
}

void MainWindow::DonateSlot()
{
	Donate dt(this);
	dt.exec();
}

void MainWindow::ImportDataSlot()
{
	ImportData imData(this);
	imData.exec();
}

void MainWindow::ExportDataSlot()
{
	ExportData exData(this);
	exData.exec();
}

void MainWindow::SetJdkPathSlot()
{
	SetJdkPath setJdk(this);
	setJdk.exec();
}

void MainWindow::SetKeySlot()
{
	SetKey setK(this);
	setK.exec();
}

void MainWindow::SourcePackSettingSlot()
{
	SourcePackSetting srcPackSetting(this);
	srcPackSetting.exec();
}

void MainWindow::DecPackSettingSlot()
{
	DecPackSetting decPackSet(this);
	decPackSet.exec();
}

void MainWindow::SetVersionSlot()
{
	SetVersionDialog setVerDialog(this);
	if (setVerDialog.exec() == QDialog::Accepted){
		mversion = PathManager::GetVersion();
		StatusTextChang();
	}
}

void MainWindow::ShowLogSlot()
{
	mplogDialog = new LogDialog (this);
	mplogDialog->setAttribute(Qt::WA_DeleteOnClose);//�Ի����Զ��ͷ���������Դ
	mplogDialog->SetText(mlog);
	misLogShowing = true;
	connect(mplogDialog, SIGNAL(destroyed(QObject *)), this, SLOT(CloseLogSlot()));
	mplogDialog->show();
}

void MainWindow::CloseLogSlot()
{
	misLogShowing = false;
}

void MainWindow::HeldSlot()
{
	QString arg = PathManager::GetDocumentsPath() + "/UserManual.chm";
	QProcess::startDetached("hh.exe",QStringList()<<arg);
}

void MainWindow::UpdataSlot()
{
	BjMessageBox::warning(this, QStringLiteral("������"), QStringLiteral("���޸��£�"), QMessageBox::Ok, QMessageBox::NoButton);
}

void MainWindow::ThreadConfigSlot()
{
	ThreadConfigDialog threadConfig(this);
	if (threadConfig.exec() == QDialog::Accepted){		
		mthreadNum = PathManager::GetThreadNum();
		StatusTextChang();
	}
}

void MainWindow::DePackToolSlot()
{
	DePack depackTool(this);
	depackTool.exec();
}

void MainWindow::CollectLog(QString log)
{
	mlog.append(log);
	if (misLogShowing){
		mplogDialog->SetText(mlog);
	}
}

void MainWindow::Httpresponse(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		// Check for redirect
		//QVariant possibleRedirectUrl =	reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		//if (!possibleRedirectUrl.toUrl().isEmpty())
		//{
		//	if (reply->url() == possibleRedirectUrl.toUrl())
		//		return; // escape possible redirect loop
		//	mnetworkManager->get(QNetworkRequest(possibleRedirectUrl.toUrl()));
		//	return;
		//}

		// first line of the currentrelease file contains a major.minor.patch version string
		QString sversion(reply->readLine());

		QStringList versiontokens = sversion.split(".");
		if (versiontokens.size() < 3)
			return;

		int major = versiontokens[0].toInt();
		int minor = versiontokens[1].toInt();
		int patch = versiontokens[2].toInt();

		bool newversion = false;
		QRegularExpression versionNumber(VERSION_NUMBERS);
		QRegularExpressionMatch mo = versionNumber.match(TOOL_VERSION);
		int majorVersion = mo.captured(1).toInt();
		int minorVersion = mo.captured(2).toInt();
		int patchVersion = mo.captured(3).toInt();
		if (major > majorVersion)
			newversion = true;
		else if (major == majorVersion)
		{
			if (minor > minorVersion)
				newversion = true;
			else if (minor == minorVersion)
			{
				if (patch > patchVersion)
					newversion = true;
			}
		}

		if (newversion)
		{
			QSettings settings(QApplication::organizationName(), QApplication::organizationName());
			bool disablecheck = settings.value("checkversion/disable", false).toBool();
			int ignmajor = settings.value("checkversion/major", 999).toInt();
			int ignminor = settings.value("checkversion/minor", 0).toInt();
			int ignpatch = settings.value("checkversion/patch", 0).toInt();

			// check if the user doesn't care about the current update
			if (!(ignmajor == major && ignminor == minor && ignpatch == patch && disablecheck))
			{
				QMessageBox msgBox;
				QPushButton *idontcarebutton = msgBox.addButton(tr("Don't show again"), QMessageBox::ActionRole);
				msgBox.addButton(QMessageBox::Ok);
				msgBox.setTextFormat(Qt::RichText);
				msgBox.setWindowTitle(tr("New version available."));
				msgBox.setText(tr("A new DB Browser for SQLite version is available (%1.%2.%3).<br/><br/>"
					"Please download at <a href='%4'>%4</a>.").arg(major).arg(minor).arg(patch).
					arg(QString(reply->readLine()).trimmed()));
				msgBox.exec();

				if (msgBox.clickedButton() == idontcarebutton)
				{
					// save that the user don't want to get bothered about this update
					settings.beginGroup("checkversion");
					settings.setValue("major", major);
					settings.setValue("minor", minor);
					settings.setValue("patch", minor);
					settings.setValue("disable", true);
					settings.endGroup();
				}
			}
		}
	}
}

void MainWindow::StatusTextChang()
{
	mstatusBar.SetText1(QStringLiteral("�߳�������%1  ��ǰ����汾:%2").arg(QString::number(mthreadNum)).arg(mversion));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	StopSlot();
}