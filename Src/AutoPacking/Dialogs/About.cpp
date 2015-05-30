#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "Dialogs/About.h"

const QString VERSION_NUMBERS = "(\\d+)\\.(\\d+)\\.(\\d+)";
const QString TOOL_VERSION = QString(TOOLS_FULL_VERSION);
const QString WEB_SITE = "http://github.com/lixiaoyu0123/AutopackingAndroid/blob/master/CurrentVersion";

About::About(QWidget *parent)
: QDialog(parent),
ui(new Ui::About)
{
	ui->setupUi(this);
	ui->lbBuildTimeDisplay->setText(GetUTCBuildTime().toString("yyyy.MM.dd HH:mm:ss") + " UTC");
	ui->lbLoadedQtDisplay->setText(QString(qVersion()));
	QRegularExpression version_number(VERSION_NUMBERS);
	QRegularExpressionMatch mo = version_number.match(TOOL_VERSION);
	QString version_text = QString("%1.%2.%3")
		.arg(mo.captured(1).toInt())
		.arg(mo.captured(2).toInt())
		.arg(mo.captured(3).toInt());
	ui->lbVersionDisplay->setText(version_text);
	QString credits = "<h4>" + QStringLiteral("����") + "</h4>" +
	"<ul><li>"+ QStringLiteral("����") + "</li></ul>" +
		"<h4>" + QStringLiteral("��ϵ��ʽ") + "</h4>" +
		"<ul>" +
		"<li>QQ:21314601</li>" +
		"<li>Email:lixiaoyu0123@163.com</li>" +
		"</ul>";
	ui->creditsDisplay->setText(credits);
}

About::~About()
{
	delete ui;
}

QDateTime About::GetUTCBuildTime()
{
	QString time_string = QString::fromLatin1(__TIME__);
	QString date_string = QString::fromLatin1(__DATE__);
	Q_ASSERT(!date_string.isEmpty());
	Q_ASSERT(!time_string.isEmpty());
	QRegularExpression date_match("(\\w{3})\\s+(\\d+)\\s+(\\d{4})");
	QRegularExpressionMatch mo = date_match.match(date_string);
	QDate date(mo.captured(3).toInt(), MonthIndexFromString(mo.captured(1)), mo.captured(2).toInt());
	return QDateTime(date, QTime::fromString(time_string, "hh:mm:ss")).toUTC();
}


// Needed because if we use the "MMM" string in the QDate::fromString
// function, it will match on localized month names, not English ones.
// The __DATE__ macro *always* uses English month names.
int About::MonthIndexFromString(const QString &three_letter_string)
{
	Q_ASSERT(three_letter_string.count() == 3);
	Q_ASSERT(three_letter_string[0].isUpper());

	if (three_letter_string == "Jan") {
		return 1;
	}

	if (three_letter_string == "Feb") {
		return 2;
	}

	if (three_letter_string == "Mar") {
		return 3;
	}

	if (three_letter_string == "Apr") {
		return 4;
	}

	if (three_letter_string == "May") {
		return 5;
	}

	if (three_letter_string == "Jun") {
		return 6;
	}

	if (three_letter_string == "Jul") {
		return 7;
	}

	if (three_letter_string == "Aug") {
		return 8;
	}

	if (three_letter_string == "Sep") {
		return 9;
	}

	if (three_letter_string == "Oct") {
		return 10;
	}

	if (three_letter_string == "Nov") {
		return 11;
	}

	if (three_letter_string == "Dec") {
		return 12;
	}

	Q_ASSERT(false);
	return 0;
}
