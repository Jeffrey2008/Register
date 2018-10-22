#include "QtGuiApplication3.h"
#include <QNetworkInterface>
#include<QProcess>
#include<md5_encode.h>
#include <string>
#include <QDateTime>
#include<iostream>
#include<QInputDialog>
#include<QSettings>
#include<QMessageBox>
using namespace std;

QtGuiApplication3::QtGuiApplication3(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton, SIGNAL(clicked()), this,SLOT(getEncodeString()));

}
//�õ�MD5����֮�����Կ
void QtGuiApplication3::getEncodeString() {
	QString CPU_ID = get_cpuId();
	QString MAC = getHostMacAddress();
	QString IP= getHostIpAddress();
	QString DISK_ID = getDiskID();
	string original= CPU_ID.toStdString()+ MAC.toStdString() + IP.toStdString() + DISK_ID.toStdString();
	//������salt���ڲ������MD5����
	string salt = "acs3@!@$#$@#)(*___*sdfkjf124654wefsdfsjdfh8ew8rywe0r983945wproiweh92385osjsdf2525s4f65";
	Md5Encode encode_obj;
	string ret_1 = encode_obj.Encode(original+salt);//�õ���ԭʼ����salt֮��ļ����ַ���
	string ret_2 = encode_obj.Encode(ret_1);
	string ret_3 = encode_obj.Encode(ret_2);
	string ret_4 = encode_obj.Encode(ret_3);
	string ret_5 = encode_obj.Encode(ret_4);
	string ret_6 = encode_obj.Encode(ret_5);
	string ret;
	transform(ret_6.begin(), ret_6.end(), back_inserter(ret), ::toupper);//ȫ����תΪ��д
	QString encodePassword = QString::fromStdString(ret);//תΪQString������ʾ
	ui.textEdit->setText(encodePassword);
	//��ע������ҵ���ֵΪactiveCode��ֵ
	QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\moemil", QSettings::NativeFormat);
	reg->setValue("Version","1.0");
	QString activeCode = reg->value("activeCode").toString();
	//�����ҵ���ֵ�ͼ���֮��ı�����Ϣ�Ա�
	if (!activeCode.isNull() && activeCode == encodePassword) {
		QMessageBox::warning(NULL, QObject::tr("success"), QObject::tr("Activie successfuly"));
	}
	else {
		QString itemText2 = QInputDialog::getText(0, tr("ActivieCode"), tr("please input:"));
		if (itemText2.toStdString() == ret) {
			//����������Կ�ͱ�������Ϣƥ�䣬����ע�����д�����ֵ
			reg->setValue("activeCode", itemText2);
			delete reg;
			QMessageBox::warning(NULL, QObject::tr("success"), QObject::tr("Activie successfuly"));
		}
		else {
			QMessageBox::warning(NULL, QObject::tr("error"), QObject::tr("ActivieCode error"));
			
		}

	}

	//�õ�ע���ʱ��
	QDateTime curDateTime = QDateTime::currentDateTime();
	QString curDateTimeStr = curDateTime.toString("yyyy-MM-dd  hh:mm:ss");
	std::cout << "current time  is " << curDateTimeStr.toStdString()<<endl<< ret<<endl;

	////���浽ע���
	//QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\qCloudSecChk", QSettings::NativeFormat);
	//reg->setValue("activeCode",encodePassword);
	//delete reg;
	//activationJudge(encodePassword);
}
//�õ����Ե�CPU��ID
QString QtGuiApplication3::get_cpuId() {
	QString cpu_id = "";
	unsigned int dwBuf[4] = { 0 };
	unsigned long long ret = 0;
	getcpuid(dwBuf, 1);
	ret = dwBuf[3];
	ret = ret << 32;

	QString str0 = QString::number(dwBuf[3], 16).toUpper();
	QString str0_1 = str0.rightJustified(8, '0');//��һ�����˼��ǰ�油0����������������������ﶼ������
	QString str1 = QString::number(dwBuf[0], 16).toUpper();
	QString str1_1 = str1.rightJustified(8, '0');//���������ǰ�油0�����򲻻���������
												 //cpu_id = cpu_id + QString::number(dwBuf[0], 16).toUpper();
	cpu_id = str0_1 + str1_1;
	return cpu_id;
}
void QtGuiApplication3::getcpuid(unsigned int CPUInfo[4], unsigned int InfoType)
{
#if defined(__GNUC__)// GCC
	__cpuid(InfoType, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)// MSVC
#if _MSC_VER >= 1400 //VC2005��֧��__cpuid
	__cpuid((int*)(void*)CPUInfo, (int)(InfoType));
#else //����ʹ��getcpuidex
	getcpuidex(CPUInfo, InfoType, 0);
#endif
#endif
}
void QtGuiApplication3::getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(_MSC_VER) // MSVC
#if defined(_WIN64) // 64λ�²�֧���������. 1600: VS2010, ��˵VC2008 SP1֮���֧��__cpuidex.
	__cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else
	if (NULL == CPUInfo)  return;
	_asm {
		// load. ��ȡ�������Ĵ���.
		mov edi, CPUInfo;
		mov eax, InfoType;
		mov ecx, ECXValue;
		// CPUID
		cpuid;
		// save. ���Ĵ������浽CPUInfo
		mov[edi], eax;
		mov[edi + 4], ebx;
		mov[edi + 8], ecx;
		mov[edi + 12], edx;
	}
#endif
#endif
}
//�õ����Ե�MAC��ַ
QString QtGuiApplication3::getHostMacAddress() {
	QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// ��ȡ��������ӿ��б�
	int nCnt = nets.count();
	QString strMacAddr = "";
	for (int i = 0; i < nCnt; i++)
	{
		// ���������ӿڱ�������������в��Ҳ��ǻػ���ַ�������������Ҫ�ҵ�Mac��ַ
		if (nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
		{
			strMacAddr = nets[i].hardwareAddress();
			break;
		}
	}

	return strMacAddr;
}
//�õ����Ե�IP��ַ
QString QtGuiApplication3::getHostIpAddress()
{
	QString strIpAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// ��ȡ��һ����������IPv4��ַ
	int nListSize = ipAddressesList.size();
	for (int i = 0; i < nListSize; ++i)
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			strIpAddress = ipAddressesList.at(i).toString();
			break;
		}
	}
	// ���û���ҵ������Ա���IP��ַΪIP
	if (strIpAddress.isEmpty())
		strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
	return strIpAddress;
}
//�õ�����Ӳ��ID
QString QtGuiApplication3::getDiskID()
{
	QProcess p;
	p.start("diskpart");
	p.waitForStarted();
	p.write("select disk 0\n detail disk\n");
	p.closeWriteChannel();
	p.waitForFinished();
	QString info = QString::fromLocal8Bit(p.readAllStandardOutput().data());
	p.kill();

	int nIdx = info.indexOf(QStringLiteral("���� ID"));
	int nIdx2 = info.indexOf("\n", nIdx);

	QString strHDD = info.mid(nIdx + 6, nIdx2 - nIdx - 6);
	strHDD = strHDD.trimmed();

	return strHDD;

}
//WINDOWS�¶�дע�����

bool QtGuiApplication3::activationJudge(QString machineCode) {

	QSettings *reg = new QSettings("HKEY_CURRENT_USER\\Software\\moemil", QSettings::NativeFormat);
	QString activeCode = reg->value("activeCode").toString();

	qDebug() << "activeCode is " << activeCode;
	// QDateTime curDateTime=QDateTime::currentDateTime();
	// QString curDateTimeStr=curDateTime.toString("yyyy-MM-dd");
	//qDebug()<<"current time  is "<<curDateTimeStr;
	if (machineCode.trimmed() != activeCode) {
		QString itemText2 = QInputDialog::getText(0, tr("ActivieCode"), tr("please input:"));

		if (itemText2 == activeCode) {
			reg->setValue("activeCode", itemText2);
			return true;
		}
		else {
			QMessageBox::warning(NULL, QObject::tr("error"), QObject::tr("ActivieCode error"));
			return false;
		}


	}
}

