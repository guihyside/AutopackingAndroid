#include "BjTableModel.h"

BjTableModel::BjTableModel(QObject *parent, QSqlDatabase db):
QSqlTableModel(parent,db)
{
}

Qt::ItemFlags BjTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;
	Qt::ItemFlags flag = QAbstractItemModel::flags(index);
	//if (index.column() != 3 && index.column() != 4){ 
		flag |= Qt::ItemIsEditable; // ���õ�Ԫ��ɱ༭,�˴�ע��,��Ԫ���޷����༭
//	}	
	return flag;
}