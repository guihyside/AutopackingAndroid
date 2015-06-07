#include <QFile>
#include <QStandardItemModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QDebug>
#include "DatabaseManager.h"
#include "PathManager.h"
#include "Dialogs/BjMessageBox.h"

DatabaseManager* DatabaseManager::mpinstance = NULL;

DatabaseManager::DatabaseManager() :
QObject(),
mpdatabase(NULL),
mptableModel(NULL),
mptableModelStr(NULL),
mptableModelRes(NULL),
mptableModelPak(NULL),
mmodelIndexStr(),
mmodelIndexRes(),
mmodelIndexPak(),
mmodelIndexAppPak()
{
	InitDataModel();
}

DatabaseManager::~DatabaseManager()
{
	Commit();
	mpdatabase->close();
	if (mptableModel != NULL){
		mptableModel->deleteLater();
		mptableModel = NULL;
	}
	if (mptableModelStr != NULL){
		mptableModelStr->deleteLater();
		mptableModelStr = NULL;
	}
	if (mptableModelRes != NULL){
		mptableModelRes->deleteLater();
		mptableModelRes = NULL;
	}
	if (mptableModelPak != NULL){
		mptableModelPak->deleteLater();
		mptableModelPak = NULL;
	}
	if (mptableModelAppPak != NULL){
		mptableModelAppPak->deleteLater();
		mptableModelAppPak = NULL;
	}
	if (mpdatabase != NULL){
		delete mpdatabase;
		mpdatabase = NULL;
	}
}

DatabaseManager *DatabaseManager::GetInstance()
{
	if (mpinstance == NULL){
		mpinstance = new DatabaseManager;
	}
	return mpinstance;
}

void DatabaseManager::Destroyed()
{
	if (mpinstance != NULL){
		delete mpinstance;
		mpinstance = NULL;
	}
}

void DatabaseManager::InitDataModel()
{
	mpdatabase = new QSqlDatabase();
	*mpdatabase = QSqlDatabase::addDatabase("QSQLITE");
	mpdatabase->setDatabaseName(PathManager::GetDataBasePath());
	if (!QFile::exists(PathManager::GetDataBasePath())){
		if (!mpdatabase->open()){
			BjMessageBox msg;
			msg.setText(QStringLiteral("���ݿⴴ��ʧ�ܣ�"));
			msg.exec();
			exit(1);
		};
		QSqlQuery query(PathManager::GetDataBasePath());
		bool isSuccess = query.exec("CREATE TABLE Channeltb ("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"ChannelID INTEGER, "
			"ChannelName VARCHAR(256), "
			"ReplaceString VARCHAR(256), "
			"ReplaceRes VARCHAR(256), "
			"ReplacePak VARCHAR(256), "
			"ReplaceAppPak VARCHAR(256), "
			"PackStatus VARCHAR(80))");

		if (!isSuccess){
			BjMessageBox msg;
			msg.setText(QStringLiteral("����Channeltbʧ�ܣ�"));
			msg.exec();
			mpdatabase->close();
			QFile::remove(PathManager::GetDataBasePath());
			exit(1);
		}

		isSuccess = query.exec("CREATE TABLE Stringtb ("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"ChanneltbID INTEGER, "
			"Path VARCHAR(256), "
			"SourceString VARCHAR(256), "
			"TargetString VARCHAR(256)) ");
		if (!isSuccess){
			BjMessageBox msg;
			msg.setText(QStringLiteral("����Stringtbʧ�ܣ�"));
			msg.exec();
			mpdatabase->close();
			QFile::remove(PathManager::GetDataBasePath());
			exit(1);
			return;
		}

		isSuccess = query.exec("CREATE TABLE Resourcetb ("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"ChanneltbID INTEGER, "
			"SourceResDir VARCHAR(256), "
			"TargetResDir VARCHAR(256)) ");
		if (!isSuccess){
			BjMessageBox msg;
			msg.setText(QStringLiteral("����Resourcetbʧ�ܣ�"));
			msg.exec();
			mpdatabase->close();
			QFile::remove(PathManager::GetDataBasePath());
			exit(1);
			return;
		}

		isSuccess = query.exec("CREATE TABLE PackageRenametb ("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"ChanneltbID INTEGER, "
			"SourcePackage VARCHAR(256), "
			"TargetPackage VARCHAR(256)) ");
		if (!isSuccess){
			BjMessageBox msg;
			msg.setText(QStringLiteral("����PackageRenametbʧ�ܣ�"));
			msg.exec();
			mpdatabase->close();
			QFile::remove(PathManager::GetDataBasePath());
			exit(1);
			return;
		}

		isSuccess = query.exec("CREATE TABLE AppPackageRenametb ("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"ChanneltbID INTEGER, "
			"SourcePackage VARCHAR(256), "
			"TargetPackage VARCHAR(256)) ");
		if (!isSuccess){
			BjMessageBox msg;
			msg.setText(QStringLiteral("����AppPackageRenametbʧ�ܣ�"));
			msg.exec();
			mpdatabase->close();
			QFile::remove(PathManager::GetDataBasePath());
			exit(1);
			return;
		}
	}
	else{
		if (!mpdatabase->open()){
			BjMessageBox msg;
			msg.setText(QStringLiteral("���ݿ��ʧ�ܣ�"));
			msg.exec();
			QFile::remove(PathManager::GetDataBasePath());
			exit(1);
		}
	}

	mptableModel = new BjTableModel(NULL, *mpdatabase);
	mptableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	mptableModel->setTable("Channeltb");
	if (!mptableModel->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModel->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModel->revertAll();//�����ɾ��������
		return;
	}
	mptableModel->setHeaderData(1, Qt::Horizontal, QStringLiteral("����ID"));
	mptableModel->setHeaderData(2, Qt::Horizontal, QStringLiteral("��������"));
	mptableModel->setHeaderData(3, Qt::Horizontal, QStringLiteral("�༭�滻�ַ���"));
	mptableModel->setHeaderData(4, Qt::Horizontal, QStringLiteral("�༭�滻��Դ"));
	mptableModel->setHeaderData(5, Qt::Horizontal, QStringLiteral("�༭�滻�ڲ�����"));
	mptableModel->setHeaderData(6, Qt::Horizontal, QStringLiteral("�༭�滻Ӧ�ð���"));
	mptableModel->setHeaderData(7, Qt::Horizontal, QStringLiteral("״̬"));


	mptableModelStr = new BjTableModel(NULL, *mpdatabase);
	mptableModelStr->setEditStrategy(QSqlTableModel::OnManualSubmit);
	mptableModelStr->setTable("Stringtb");
	if (!mptableModelStr->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelStr->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelStr->revertAll();//�����ɾ��������
		return;
	}
	mptableModelStr->setHeaderData(2, Qt::Horizontal, QStringLiteral("�ļ����·��"));
	mptableModelStr->setHeaderData(3, Qt::Horizontal, QStringLiteral("ԭ�ڵ�"));
	mptableModelStr->setHeaderData(4, Qt::Horizontal, QStringLiteral("�滻��ڵ�"));

	mptableModelRes = new BjTableModel(NULL, *mpdatabase);
	mptableModelRes->setEditStrategy(QSqlTableModel::OnManualSubmit);
	mptableModelRes->setTable("Resourcetb");
	if (!mptableModelRes->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelRes->revertAll();//�����ɾ��������
		return;
	}
	mptableModelRes->setHeaderData(2, Qt::Horizontal, QStringLiteral("ԭ��Դ����Ŀ¼"));
	mptableModelRes->setHeaderData(3, Qt::Horizontal, QStringLiteral("�滻��Դ����Ŀ¼"));

	mptableModelPak = new BjTableModel(NULL, *mpdatabase);
	mptableModelPak->setEditStrategy(QSqlTableModel::OnManualSubmit);
	mptableModelPak->setTable("PackageRenametb");
	if (!mptableModelPak->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelPak->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelPak->revertAll();//�����ɾ��������
		return;
	}
	mptableModelPak->setHeaderData(2, Qt::Horizontal, QStringLiteral("ԭ����"));
	mptableModelPak->setHeaderData(3, Qt::Horizontal, QStringLiteral("�滻��İ���"));

	mptableModelAppPak = new BjTableModel(NULL, *mpdatabase);
	mptableModelAppPak->setEditStrategy(QSqlTableModel::OnManualSubmit);
	mptableModelAppPak->setTable("AppPackageRenametb");
	if (!mptableModelAppPak->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelAppPak->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelAppPak->revertAll();//�����ɾ��������
		return;
	}
	mptableModelAppPak->setHeaderData(2, Qt::Horizontal, QStringLiteral("ԭ����"));
	mptableModelAppPak->setHeaderData(3, Qt::Horizontal, QStringLiteral("�滻��İ���"));
}

void DatabaseManager::AddRow()
{
	int rowNum = mptableModel->rowCount();//��ñ������
	QSqlRecord record = mptableModel->record();
	QSqlField fieldStr("ReplaceString", QVariant::Char);
	QSqlField fieldRes("ReplaceRes", QVariant::Char);
	QSqlField fieldPak("ReplacePak", QVariant::Char);
	QSqlField fieldAppPak("ReplaceAppPak", QVariant::Char);
	fieldStr.setValue(QStringLiteral("˫���༭"));
	fieldRes.setValue(QStringLiteral("˫���༭"));
	fieldPak.setValue(QStringLiteral("˫���༭"));
	fieldAppPak.setValue(QStringLiteral("˫���༭"));
	record.append(fieldStr);
	record.append(fieldRes);
	record.append(fieldPak);
	record.append(fieldAppPak);

	if (!mptableModel->insertRecord(rowNum, record)){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModel->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModel->revertAll();//�����ɾ��������
		return;
	}

	if (!mptableModel->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModel->lastError().text()));
		mptableModel->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::DelRow(QModelIndexList &selecteds)
{
	foreach(QModelIndex index, selecteds)
	{
		int curRow = index.row(); //ɾ�����б�ѡ�е���		
		if (!mptableModel->removeRow(curRow)){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModel->lastError().text()));
			mptableModel->revertAll();//�����ɾ��������
			return;
		}
		QString id = mptableModel->record(curRow).value("ID").toString();
		mptableModelStr->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
		if (!mptableModelStr->select()){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelStr->lastError().text()));
			mptableModelStr->revertAll();//�����ɾ��������
		}
		DeleteAll(*mptableModelStr);

		mptableModelRes->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
		if (!mptableModelRes->select()){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()));
			mptableModelRes->revertAll();//�����ɾ��������
		}
		DeleteAll(*mptableModelRes);

		mptableModelPak->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
		if (!mptableModelPak->select()){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelPak->lastError().text()));
			mptableModelPak->revertAll();//�����ɾ��������
		}
		DeleteAll(*mptableModelPak);

		mptableModelAppPak->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
		if (!mptableModelAppPak->select()){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelAppPak->lastError().text()));
			mptableModelAppPak->revertAll();//�����ɾ��������
		}
		DeleteAll(*mptableModelAppPak);
	}

	if (!mptableModel->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModel->lastError().text()));
		mptableModel->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::AddRowStr()
{
	int rowNum = mptableModelStr->rowCount();//��ñ������
	QString id = mptableModel->record(mmodelIndexStr.row()).value("ID").toString();
	QSqlRecord record = mptableModelStr->record();
	QSqlField fieldChannltbId("ChanneltbID", QVariant::Int);
	fieldChannltbId.setValue(id);
	record.append(fieldChannltbId);

	if (!mptableModelStr->insertRecord(rowNum, record)){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelStr->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelStr->revertAll();//�����ɾ��������
		return;
	}

	if (!mptableModelStr->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelStr->lastError().text()));
		mptableModelStr->revertAll();//�����ɾ��������
	}

}

void DatabaseManager::DelRowStr(QModelIndexList &selecteds)
{
	foreach(QModelIndex index, selecteds)
	{
		int curRow = index.row(); //ɾ�����б�ѡ�е���		
		if (!mptableModelStr->removeRow(curRow)){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelStr->lastError().text()));
			mptableModelStr->revertAll();//�����ɾ��������
			return;
		}
	}

	if (!mptableModelStr->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelStr->lastError().text()));
		mptableModelStr->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::AddRowRes()
{
	int rowNum = mptableModelRes->rowCount();//��ñ������
	QString id = mptableModel->record(mmodelIndexRes.row()).value("ID").toString();
	QSqlRecord record = mptableModelRes->record();
	QSqlField fieldChannltbId("ChanneltbID", QVariant::Int);
	fieldChannltbId.setValue(id);
	record.append(fieldChannltbId);


	if (!mptableModelRes->insertRecord(rowNum, record)){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelRes->revertAll();//�����ɾ��������
		return;
	}

	if (!mptableModelRes->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()));
		mptableModelRes->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::DelRowRes(QModelIndexList &selecteds)
{
	foreach(QModelIndex index, selecteds)
	{
		int curRow = index.row(); //ɾ�����б�ѡ�е���		
		if (!mptableModelRes->removeRow(curRow)){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()));
			mptableModelRes->revertAll();//�����ɾ��������
			return;
		}
	}

	if (!mptableModelRes->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()));
		mptableModelRes->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::AddRowPak()
{
	int rowNum = mptableModelPak->rowCount();//��ñ������
	QString id = mptableModel->record(mmodelIndexPak.row()).value("ID").toString();
	QSqlRecord record = mptableModelPak->record();
	QSqlField fieldChannltbId("ChanneltbID", QVariant::Int);
	fieldChannltbId.setValue(id);
	record.append(fieldChannltbId);


	if (!mptableModelPak->insertRecord(rowNum, record)){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelPak->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelPak->revertAll();//�����ɾ��������
		return;
	}

	if (!mptableModelPak->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelPak->lastError().text()));
		mptableModelPak->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::DelRowPak(QModelIndexList &selecteds)
{
	foreach(QModelIndex index, selecteds)
	{
		int curRow = index.row(); //ɾ�����б�ѡ�е���		
		if (!mptableModelPak->removeRow(curRow)){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelPak->lastError().text()));
			mptableModelPak->revertAll();//�����ɾ��������
			return;
		}
	}

	if (!mptableModelPak->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelPak->lastError().text()));
		mptableModelPak->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::AddRowAppPak()
{
	int rowNum = mptableModelAppPak->rowCount();//��ñ������
	QString id = mptableModel->record(mmodelIndexAppPak.row()).value("ID").toString();
	QSqlRecord record = mptableModelAppPak->record();
	QSqlField fieldChannltbId("ChanneltbID", QVariant::Int);
	fieldChannltbId.setValue(id);
	record.append(fieldChannltbId);


	if (!mptableModelAppPak->insertRecord(rowNum, record)){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelAppPak->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModelAppPak->revertAll();//�����ɾ��������
		return;
	}

	if (!mptableModelAppPak->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelAppPak->lastError().text()));
		mptableModelAppPak->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::DelRowAppPak(QModelIndexList &selecteds)
{
	foreach(QModelIndex index, selecteds)
	{
		int curRow = index.row(); //ɾ�����б�ѡ�е���		
		if (!mptableModelAppPak->removeRow(curRow)){
			BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelAppPak->lastError().text()));
			mptableModelAppPak->revertAll();//�����ɾ��������
			return;
		}
	}

	if (!mptableModelAppPak->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelAppPak->lastError().text()));
		mptableModelAppPak->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::DeleteAll(QSqlTableModel &model)
{
	int rowNum = model.rowCount();
	if (rowNum == 0){
		return;
	}

	if (!model.removeRows(0, rowNum)){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(model.lastError().text()));
		model.revertAll();//�����ɾ��������
	 }

	if (!model.submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(model.lastError().text()));
		model.revertAll();//�����ɾ��������
	}
}

bool DatabaseManager::isDatabaseEmpty()
{
	if (mptableModel->rowCount() == 0){
		return true;
	}
	return false;
}

BjTableModel *DatabaseManager::GetTableModel()
{
	return mptableModel;
}

QSqlDatabase *DatabaseManager::GetDatabase()
{
	return mpdatabase;
}

BjTableModel *DatabaseManager::GetTableModelStr(const QModelIndex &modelIndexStr)
{
	mmodelIndexStr = modelIndexStr;
	QString id = mptableModel->record(modelIndexStr.row()).value("ID").toString();
	mptableModelStr->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	if (!mptableModelStr->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModel->revertAll();//�����ɾ��������
	}
	return mptableModelStr;
}

BjTableModel *DatabaseManager::GetTableModelRes(const QModelIndex &modelIndexRes)
{
	mmodelIndexRes = modelIndexRes;
	QString id = mptableModel->record(mmodelIndexRes.row()).value("ID").toString();
	mptableModelRes->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	if (!mptableModelRes->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelRes->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModel->revertAll();//�����ɾ��������
	}
	return mptableModelRes;
}

BjTableModel *DatabaseManager::GetTableModelPak(const QModelIndex &modelIndexPak)
{
	mmodelIndexPak = modelIndexPak;
	QString id = mptableModel->record(mmodelIndexPak.row()).value("ID").toString();
	mptableModelPak->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	if (!mptableModelPak->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelPak->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModel->revertAll();//�����ɾ��������
	}
	return mptableModelPak;
}

BjTableModel *DatabaseManager::GetTableModelAppPak(const QModelIndex &modelIndexAppPak)
{
	mmodelIndexAppPak = modelIndexAppPak;
	QString id = mptableModel->record(mmodelIndexAppPak.row()).value("ID").toString();
	mptableModelAppPak->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	if (!mptableModelAppPak->select()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModelAppPak->lastError().text()), QMessageBox::Ok, QMessageBox::NoButton);
		mptableModel->revertAll();//�����ɾ��������
	}
	return mptableModelAppPak;
}

int DatabaseManager::GetTableModelAppPakCounts()
{
	return mptableModelAppPak->rowCount();
}

BjTableModel *DatabaseManager::GetTableModelStr()
{
	return mptableModelStr;
}

BjTableModel *DatabaseManager::GetTableModelRes()
{
	return mptableModelRes;
}

BjTableModel *DatabaseManager::GetTableModelPak()
{
	return mptableModelPak;
}

BjTableModel *DatabaseManager::GetTableModelAppPak()
{
	return mptableModelAppPak;
}

void DatabaseManager::ChangStatInDatabase(int row, QString &status)
{
	QSqlRecord record = mptableModel->record(row);
	record.setValue("PackStatus", status);
	if (!mptableModel->setRecord(row, record)){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModel->lastError().text()));
		mptableModel->revertAll();//�����ɾ��������
	}
	if (!mptableModel->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1").arg(mptableModel->lastError().text()));
		mptableModel->revertAll();//�����ɾ��������
	}
}

void DatabaseManager::ReadyData(QString &id, QList<ReplaceStrTable> &strTable, QList<ReplaceResTable>  &resTable, QList<ReplacePakTable>  &pakTable, QList<ReplaceAppPakTable> &appPakTable)
{
	mptableModelStr->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	int rowNum = mptableModelStr->rowCount();
	for (int i = 0; i < rowNum; i++)
	{
		QString path = mptableModelStr->record(i).value("Path").toString().trimmed();
		QString srcStr = mptableModelStr->record(i).value("SourceString").toString();
		QString destStr = mptableModelStr->record(i).value("TargetString").toString();
		ReplaceStrTable reStrTable;
		reStrTable.SetFolder(path);
		reStrTable.SetSrcStr(srcStr);
		reStrTable.SetDestStr(destStr);
		strTable.push_back(reStrTable);
	}

	mptableModelRes->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	rowNum = mptableModelRes->rowCount();
	for (int i = 0; i < rowNum; i++)
	{
		QString srcDir = mptableModelRes->record(i).value("SourceResDir").toString().trimmed();
		QString destDir = mptableModelRes->record(i).value("TargetResDir").toString().trimmed();
		ReplaceResTable reResTable;
		reResTable.SetFolderSrc(srcDir);
		reResTable.SetFolderDest(destDir);
		resTable.push_back(reResTable);
	}

	mptableModelPak->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	rowNum = mptableModelPak->rowCount();
	for (int i = 0; i < rowNum; i++)
	{
		QString srcPak = mptableModelPak->record(i).value("SourcePackage").toString().trimmed();
		QString destPak = mptableModelPak->record(i).value("TargetPackage").toString().trimmed();
		ReplacePakTable rePakTable;
		rePakTable.SetSrcPakName(srcPak);
		rePakTable.SetDestPakName(destPak);
		pakTable.push_back(rePakTable);
	}

	mptableModelAppPak->setFilter(QString("ChanneltbID=\'%1\'").arg(id));
	rowNum = mptableModelAppPak->rowCount();
	for (int i = 0; i < rowNum; i++)
	{
		QString srcPak = mptableModelAppPak->record(i).value("SourcePackage").toString().trimmed();
		QString destPak = mptableModelAppPak->record(i).value("TargetPackage").toString().trimmed();
		ReplaceAppPakTable reAppPakTable;
		reAppPakTable.SetSrcPakName(srcPak);
		reAppPakTable.SetDestPakName(destPak);
		appPakTable.push_back(reAppPakTable);
	}
}

void DatabaseManager::ExportData(QString &fileName)
{
	Commit();
	mpdatabase->close();
	//�ͷ�DataBase������ReloadData���ʹ�ã��ͷ�֮����ReloadData�ؽ�
	if (mpdatabase != NULL){
		delete mpdatabase;
		mpdatabase = NULL;
	}
	QSqlDatabase::removeDatabase("QSQLITE");

	if (!PathManager::CopyFile(PathManager::GetDataBasePath(), fileName, true)){
		BjMessageBox::warning(NULL, QStringLiteral("�������ݳ���"), QStringLiteral("�������ݴ���!"), QMessageBox::Ok, QMessageBox::NoButton);
		ReloadData();
		return;
	}
	BjMessageBox::information(NULL, QStringLiteral("�����ɹ���"), QStringLiteral("�����ɹ���"), QMessageBox::Ok, QMessageBox::NoButton);
	ReloadData();
}

void DatabaseManager::ImportData(QString &fileName)
{
	Commit();
	mpdatabase->close();
	//�ͷ�DataBase������ReloadData���ʹ�ã��ͷ�֮����ReloadData�ؽ�
	if (mpdatabase != NULL){
		delete mpdatabase;
		mpdatabase = NULL;
	}	
	QSqlDatabase::removeDatabase("QSQLITE");

	if (!PathManager::CopyFile(fileName,PathManager::GetDataBasePath(), true)){
		BjMessageBox::warning(NULL, QStringLiteral("�������ݳ���"), QStringLiteral("�������ݴ���!"), QMessageBox::Ok, QMessageBox::NoButton);
		ReloadData();
		return;
	}
	BjMessageBox::information(NULL, QStringLiteral("����ɹ���"), QStringLiteral("����ɹ���"), QMessageBox::Ok, QMessageBox::NoButton);
	ReloadData();
}

void DatabaseManager::ReloadData()
{
	if (mptableModel != NULL){
		mptableModel->deleteLater();
		mptableModel = NULL;
	}
	if (mptableModelStr != NULL){
		mptableModelStr->deleteLater();
		mptableModelStr = NULL;
	}
	if (mptableModelRes != NULL){
		mptableModelRes->deleteLater();
		mptableModelRes = NULL;
	}
	if (mptableModelPak != NULL){
		mptableModelPak->deleteLater();
		mptableModelPak = NULL;
	}
	if (mptableModelAppPak != NULL){
		mptableModelAppPak->deleteLater();
		mptableModelAppPak = NULL;
	}

	InitDataModel();
	emit ReloadDataSignal();
}

void DatabaseManager::Commit()
{
	if (!mpdatabase->transaction()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("��ʼ���������!"), QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	if (!mptableModel->submitAll() || !mptableModelStr->submitAll() || !mptableModelRes->submitAll() || !mptableModelPak->submitAll() || !mptableModelAppPak->submitAll()){
		BjMessageBox::warning(NULL, QStringLiteral("���ݿ����"), QStringLiteral("���ݿ����: %1 %2 %3 %4")
			.arg(mptableModel->lastError().text())
			.arg(mptableModelStr->lastError().text())
			.arg(mptableModelRes->lastError().text())
			.arg(mptableModelPak->lastError().text())
			.arg(mptableModelAppPak->lastError().text()));
		mpdatabase->rollback();//�ع�
		return;
	}
	mpdatabase->commit();//�ύ
}