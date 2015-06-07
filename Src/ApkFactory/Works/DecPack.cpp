#include <QProcess>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include "DecPack.h"
#include "Model/PathManager.h"
#include "Model/Tools.h"

DecPack::DecPack(QObject *parent) :Pack(parent),
mtmpUnpacketPath("")
{
}

DecPack::~DecPack()
{
}

void DecPack::Init(QString &inPath, QString &outPath, QString &channelId, QString &channelName, QString &channeltbID,
	QList<ReplaceStrTable> &strTableList, QList<ReplaceResTable> &resTableList, QList<ReplacePakTable> &pakTableList, QList<ReplaceAppPakTable> &appPakTableList, int taskId)
{
	if (inPath.isEmpty() || outPath.isEmpty()){
		emit GenerateError(QStringLiteral("error:�������·��Ϊ�գ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (channelId.isEmpty() && channelName.isEmpty()){
		emit GenerateError(QStringLiteral("error:����id����������Ϊ�գ�����ID: %1, ������ : %2\n").arg(mchannelId).arg(mchannelName));
		emit FinishSignal(1, mtaskId);
		return;
	}

	mchannelId = channelId;
	mchannelName = channelName;
	mtaskId = taskId;
	mstrTableList = strTableList;
	mresTableList = resTableList;
	mpakTableList = pakTableList;
	mappPakTableList = appPakTableList;
	minputPath = inPath;
	moutputPath = outPath;
	mchanneltbId = channeltbID;
}

void DecPack::run()
{
	mpprocess = new QProcess(NULL);
	if (!CreatPath(moutputPath, mchannelId, mchannelName, mchanneltbId)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!Unpacket(minputPath, mtmpUnpacketPath, *mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!CheckError(*mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!PathManager::RewriteApktoolYml(mtmpUnpacketPath)){
		emit GenerateError(QStringLiteral("error:��дapktool.yml��������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!ReplaceStrByTable(mtmpUnpacketPath)){
		emit GenerateError(QStringLiteral("error:�滻�ַ�����������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
	}

	if (!ReplaceResByTable(mtmpUnpacketPath)){
		emit GenerateError(QStringLiteral("error:�滻��Դ��������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
	}

	if (!ReplacePakByTable(mtmpUnpacketPath)){
		emit GenerateError(QStringLiteral("error:�滻�ڲ�������������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!ReplaceAppPakByTable(mtmpUnpacketPath)){
		emit GenerateError(QStringLiteral("error:�滻Ӧ�ð�����������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!Dopacket(mtmpUnpacketPath, mtmpSignFile, *mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!CheckError(*mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!SignPacket(mtmpSignFile, mtmpSignFile, *mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!CheckError(*mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!Zipalign(*mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	if (!CheckError(*mpprocess)){
		KillTask();
		emit FinishSignal(1, mtaskId);
		return;
	}

	KillTask();
	emit FinishSignal(0, mtaskId);
}

bool DecPack::CreatPath(QString &outPath, QString &channelId, QString &channelName, QString &channeltbId)
{
	if (outPath.endsWith("/")){
		if (PathManager::GetVersion().trimmed().isEmpty()){
			moutFile = outPath + channelName + "_" + channelId + ".apk";
		}
		else{
			moutFile = outPath + channelName + "_" + PathManager::GetVersion().trimmed() + "_" + channelId + ".apk";
		}
	}
	else{
		if (PathManager::GetVersion().trimmed().isEmpty()){
			moutFile = outPath + "/" + channelName + "_" + channelId + ".apk";
		}
		else{
			moutFile = outPath + "/" + channelName + "_" + PathManager::GetVersion().trimmed() + "_" + channelId + ".apk";
		}		
	}

	mtmpPath = PathManager::GetTmpPath().trimmed() + QStringLiteral("/") + channeltbId;
	QString unpackPath = mtmpPath + QStringLiteral("/unpack");
	QString signPath = mtmpPath + QStringLiteral("/sign");

	QDir tmpDir(unpackPath);
	if (tmpDir.exists(unpackPath)){
		if (!PathManager::RemoveDir(unpackPath)){
			return false;
		}
	}

	if (tmpDir.exists(signPath)){
		if (!PathManager::RemoveDir(signPath)){
			return false;
		}
	}

	PathManager::CreatDir(unpackPath);
	PathManager::CreatDir(signPath);
	mtmpUnpacketPath = unpackPath;
	mtmpSignFile = signPath + "/" + channelName + "_" + PathManager::GetVersion().trimmed() + channelId + "_" + ".apk";
	return true;
}

bool DecPack::ReplacePakByTable(QString &path)
{
	for (QList<ReplacePakTable>::iterator ite = mpakTableList.begin(); ite != mpakTableList.end(); ite++)
	{
		switch (PathManager::ReplacePakInDec(path, ite->GetSrcPakName().trimmed(), ite->GetDestPakName().trimmed()))
		{
		case 0:
			break;
		case 1:
			emit GenerateError(QStringLiteral("error:�滻��������,ԭ���������ڣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 2:
			emit GenerateError(QStringLiteral("error:�滻��������,��������������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 3:
			emit GenerateError(QStringLiteral("error:�滻��������,Ŀ�İ����Ѿ����ڣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 4:
			emit GenerateError(QStringLiteral("error:�滻��������,�滻�������̳�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		}
	}
	return true;
}

bool DecPack::ReplaceAppPakByTable(QString &path)
{
	for (QList<ReplaceAppPakTable>::iterator ite = mappPakTableList.begin(); ite != mappPakTableList.end(); ite++)
	{
		switch (PathManager::ReplaceAppPakInDec(path, ite->GetSrcPakName().trimmed(), ite->GetDestPakName().trimmed()))
		{
		case 0:
			break;
		case 1:
			emit GenerateError(QStringLiteral("error:�滻��������,ԭ���������ڣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 2:
			emit GenerateError(QStringLiteral("error:�滻��������,��������������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 3:
			emit GenerateError(QStringLiteral("error:�滻��������,Ŀ�İ����Ѿ����ڣ�����ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 4:
			emit GenerateError(QStringLiteral("error:�滻��������,�滻�������̳�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		case 5:
			emit GenerateError(QStringLiteral("error:�滻App��������,�滻�������̳�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		}
	}
	return true;
}

bool DecPack::Unpacket(QString &inPath, QString &outPath, QProcess &pprocess)
{
	QString apkTool = QStringLiteral("\"apktool.bat\"");
	QStringList param;
	param << QString("d") << QString("-f") << QString("-o") << "\"" + outPath + "\"" << "\"" + inPath + "\"";
	if (!Tools::ExecuteCmd(apkTool, param, pprocess, PathManager::GetToolPath().trimmed())){
		emit GenerateError(QStringLiteral("error:����ִ�д�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		return false;
	}
	return true;
}

bool DecPack::Dopacket(QString &inPath, QString &outPath, QProcess &pprocess)
{
	QString apkTool = QStringLiteral("\"apktool.bat\"");
	QStringList param;
	param << QString("b") << "\"" + inPath + "\"" << "-o" << "\"" + outPath + "\"";
	if (!Tools::ExecuteCmd(apkTool, param, pprocess, PathManager::GetToolPath().trimmed())){
		emit GenerateError(QStringLiteral("error:����ִ�д�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		return false;
	}
	return true;
}

bool DecPack::SignPacket(QString inPath, QString outPath, QProcess &pprocess)
{
	QString exe = QStringLiteral("\"jarsigner.exe\"");
	QStringList param;
	param << QStringLiteral("-sigalg") << PathManager::GetSigalg().trimmed() << QStringLiteral("-verbose") << QStringLiteral("-digestalg")
		<< PathManager::GetDigestalg().trimmed() << QStringLiteral("-keystore") << "\"" + PathManager::GetKeyPath().trimmed() + "\"" << QStringLiteral("-storepass") << PathManager::GetPasswd()
		<< QStringLiteral("-keypass") << PathManager::GetAliasesPasswd() << "\"" + outPath + "\"" << PathManager::GetKeyAliases().trimmed();
	if (!Tools::ExecuteCmd(exe, param, pprocess, PathManager::GetJdkPath())){
		emit GenerateError(QStringLiteral("error:����ִ�д�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		return false;
	}
	return true;
}

bool DecPack::Zipalign(QProcess &pprocess)
{
	if (QFile::exists(moutFile)){
		if (!QFile::remove(moutFile)){
			emit GenerateError(QStringLiteral("error:�������Ŀ¼ԭ����������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
			return false;
		}
	}
	QString exe = QStringLiteral("\"zipalign.exe\"");
	QStringList param;
	param << QStringLiteral("-f") << QStringLiteral("-v")<< QStringLiteral("4") << "\"" + mtmpSignFile + "\"" << "\"" + moutFile + "\"";
	if (!Tools::ExecuteCmd(exe, param, pprocess, PathManager::GetToolPath().trimmed())){
		emit GenerateError(QStringLiteral("error:����ִ�д�������ID:%1,������:%2\n").arg(mchannelId).arg(mchannelName));
		return false;
	}
	return true;
}