#ifndef IMXPROVIDER_H
#define IMXPROVIDER_H

#include <GravityFingerprints/FingerprintProviderPlugin>

class iMXProvider : public Gravity::FingerprintProviderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID FingerprintProviderInterface_iid)
    Q_INTERFACES(Gravity::FingerprintProviderPlugin)

public:
    explicit iMXProvider(QObject *parent = nullptr);
    virtual ~iMXProvider();

    virtual QByteArray initHardwareSerialNumber() override final;

private:
};

#endif // IMXPROVIDER_H
