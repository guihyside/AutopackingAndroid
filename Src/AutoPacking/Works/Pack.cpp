#include <QDir>
#include <QTextCodec>
#include "Pack.h"
#include "Model/PathManager.h"

Pack::Pack(QObject *parent) :
QThread(parent),
moutFile(""),
mtmpSignFile(""),
mchannelId(""),
mchannelName(""),
mtaskId(0),
mpprocess(NULL)
{

}

Pack::~Pack()
{
	if (mpprocess != NULL){
		mpprocess->terminate();
		mpprocess->deleteLater();
	}
	quit();
	wait();
}

void Pack::Stop()
{
	if (mpprocess != NULL){
		QProcess killer;
		killer.start("taskkill", QStringList() << "/f" << "/im" << "java.exe");
		if (!killer.waitForStarted())
			return;
		if (!killer.waitForFinished())
			return;
		mpprocess->terminate();
		mpprocess->deleteLater();
		mpprocess = NULL;
	}
	this->terminate();
	this->wait();
	if (!PathManager::RemoveDir(mtmpPath)){
		emit GenerateError(QStringLiteral("error:��������������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
	}
	emit FinishSignal(2, mtaskId);
}

void Pack::KillTask()
{
	if (mpprocess != NULL){
		mpprocess->close();
		mpprocess->deleteLater();
		mpprocess = NULL;
	}
	if (!PathManager::RemoveDir(mtmpPath)){
		emit GenerateError(QStringLiteral("error:��������������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
	}
}

int Pack::GetTaskId()
{
	return mtaskId;
}

bool Pack::ReplaceStrByTable(QString &path)
{
	for (QList<ReplaceStrTable>::iterator ite = mstrTableList.begin(); ite != mstrTableList.end(); ite++)
	{
		QString findFile;
		QString file = ite->GetFolder().trimmed();
		if (file.isEmpty() || ite->GetSrcStr().isEmpty() || ite->GetDestStr().isEmpty()){
			emit GenerateError(QStringLiteral("error:Ҫ�滻�ַ������ļ������ڻ��ļ����滻���ַ�����Ŀ���ַ���Ϊ�գ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		}

		if (file.replace("\\", "/").startsWith("/")){
			findFile = path + file;
		}
		else{
			findFile = path + "/" + file;
		}

		if (!QFile::exists(findFile)){
			emit GenerateError(QStringLiteral("error:Ҫ�滻�ַ������ļ������ڣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		}

		switch (PathManager::ReplaceStrStrict(findFile, ite->GetSrcStr().trimmed(), ite->GetDestStr().trimmed()))
		{
		case 1:
			emit GenerateError(QStringLiteral("error:�滻�ַ���ʧ�ܣ�δ����ԭ�ַ�����ָ�����ļ��У� ����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 2:
			emit GenerateError(QStringLiteral("error:�滻�ַ���ʧ�ܣ�ָ���ļ������� ����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 3:
			emit GenerateError(QStringLiteral("error:�滻�ַ���ʧ�ܣ��������� ����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		}
	}
	return true;
}

bool Pack::ReplaceResByTable(QString &path)
{
	for (QList<ReplaceResTable>::iterator ite = mresTableList.begin(); ite != mresTableList.end(); ite++)
	{
		QString destAbsolutelyStr;
		QString srcStr;
		QString destStr;
		srcStr = ite->GetFolderSrc().trimmed();
		destStr = ite->GetFolderDest().trimmed();
		destStr.replace("\\", "/");
		if (srcStr.isEmpty() || destStr.isEmpty()){
			emit GenerateError(QStringLiteral("error:ԭ��Դ�ļ��л�Ŀ���ļ���δ���ã�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		}
		if (srcStr.replace("\\", "/").startsWith("/")){
			destAbsolutelyStr = path + destStr;
		}
		else{
			destAbsolutelyStr = path + "/" + destStr;
		}

		QFileInfo finfoSrc(srcStr);
		QFileInfo finfoDest(destAbsolutelyStr);

		if (finfoSrc.isFile()){
			if (!finfoSrc.exists()){
				emit GenerateError(QStringLiteral("error:��ԴԴ�ļ������ڣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
				return false;
			}
			if (finfoDest.isFile()){
				if (!PathManager::CopyFile(srcStr, destAbsolutelyStr, true)){
					emit GenerateError(QStringLiteral("error:��Դ�ļ�������������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
					return false;
				}
			}
			else if (finfoDest.isDir()){
				int pos = srcStr.lastIndexOf("/");
				QString fileName = srcStr.mid(pos);
				if (!PathManager::CopyFile(srcStr, destAbsolutelyStr + fileName, true)){
					emit GenerateError(QStringLiteral("error:��Դ�ļ�������������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
					return false;
				}
			}
			else{
				emit GenerateError(QStringLiteral("error:��ԴĿ�����ô�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
				return false;
			}

		}
		else if (finfoSrc.isDir()){
			if (!finfoSrc.isDir()){
				emit GenerateError(QStringLiteral("error:��ԴԴĿ¼�����ڣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
				return false;
			}
			if (finfoDest.isFile()){
				emit GenerateError(QStringLiteral("error:��ԴԴΪĿ¼��Ŀ�����ò�Ӧ��Ϊ�ļ�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
				return false;
			}
			else if (finfoDest.isDir()){
				if (!PathManager::CopyDir(srcStr, destAbsolutelyStr, true)){
					emit GenerateError(QStringLiteral("error:��ԴĿ¼����ʧ�ܣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
					return false;
				}
			}
			else{
				emit GenerateError(QStringLiteral("error:��ԴĿ�����ô�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
				return false;
			}
		}
	}
	return true;
}

bool Pack::CheckError(QProcess &pprocess)
{
	QTextCodec *gbk = QTextCodec::codecForName("GBK");
	QString error = gbk->toUnicode(pprocess.readAllStandardError());
	if (error.toLower().contains("error:")
		|| error.toLower().contains("exception:")
		|| error.toLower().contains(" error")
		|| error.toLower().contains(" exception")
		|| error.toLower().contains("build failed")){
		QString standardOut = gbk->toUnicode(pprocess.readAllStandardOutput());
		emit GenerateError(error.append(standardOut));
		return false;
	}
	return true;
}