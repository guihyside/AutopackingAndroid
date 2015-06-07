#include "TableContralAdapterAppPak.h"
#include "Model/DatabaseManager.h"
#include "Dialogs/BjMessageBox.h"

void TableContralAdapterAppPak::AddRow()
{
	if (DatabaseManager::GetInstance()->GetTableModelAppPakCounts() >= 1){
		BjMessageBox::information(NULL, QStringLiteral("������ʾ"), QStringLiteral("Ӧ�ð���ֻ��һ��������ֻ���滻һ��"), QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	DatabaseManager::GetInstance()->AddRowAppPak();
}

void TableContralAdapterAppPak::DelRow(QModelIndexList &selecteds)
{
	DatabaseManager::GetInstance()->DelRowAppPak(selecteds);
}

void TableContralAdapterAppPak::DoubleClick(const QModelIndex &index)
{
}