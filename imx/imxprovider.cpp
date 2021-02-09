/*
 * Shamelessly copied from https://community.freescale.com/message/444770
 */

#include "imxprovider.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>

#define IMX6_UID_WORD1 "/sys/fsl_otp/HW_OCOTP_CFG0"
#define IMX6_UID_WORD2 "/sys/fsl_otp/HW_OCOTP_CFG1"

iMXProvider::iMXProvider(QObject *parent)
    : Gravity::FingerprintProviderPlugin(parent)
{
}

iMXProvider::~iMXProvider()
{
}

QByteArray iMXProvider::initHardwareSerialNumber()
{
    QByteArray ret;

    if (!QFile::exists(QLatin1String(IMX6_UID_WORD1)) || !QFile::exists(QLatin1String(IMX6_UID_WORD2))) {
        qWarning() << "Couldn't find Freescale OTP information in /sys! Is your kernel configured correctly?";
        return QByteArray();
    }

    QFile word1(QLatin1String(IMX6_UID_WORD1));
    if (!word1.open(QIODevice::ReadOnly)) {
        qWarning() << "Error opening file: " << word1.error();
    }
    ret.append(word1.readAll());

    QFile word2(QLatin1String(IMX6_UID_WORD2));
    if (!word2.open(QIODevice::ReadOnly)) {
        qWarning() << "Error opening file: " << word2.error();
    }
    ret.append(word2.readAll());

    return ret;
}

