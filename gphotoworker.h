#ifndef GPHOTOWORKER_H
#define GPHOTOWORKER_H

#include <memory>

#include <QCamera>
#include <QElapsedTimer>
#include <QMutex>
#include <QObject>

#include <gphoto2/gphoto2-abilities-list.h>
#include <gphoto2/gphoto2-context.h>
#include <gphoto2/gphoto2-port-info-list.h>

class GPhotoCamera;

using CameraAbilitiesListPtr = std::unique_ptr<CameraAbilitiesList, int (*)(CameraAbilitiesList*)>;
using GPContextPtr = std::unique_ptr<GPContext, void (*)(GPContext*)>;
using GPPortInfoListPtr = std::unique_ptr<GPPortInfoList, int (*)(GPPortInfoList*)>;

struct GPhotoDevices final
{
    GPhotoDevices();

    QList<QByteArray> paths;
    QList<QByteArray> models;
    QList<QByteArray> names;
    QByteArray defaultCameraName;
    GPPortInfoListPtr portInfoList;
    QElapsedTimer cacheAgeTimer;
};

class GPhotoWorker final : public QObject
{
    Q_OBJECT
public:
    GPhotoWorker();
    ~GPhotoWorker();

    GPhotoWorker(GPhotoWorker&&) = delete;
    GPhotoWorker& operator=(GPhotoWorker&&) = delete;

    Q_INVOKABLE bool init();

    Q_INVOKABLE QList<QByteArray> cameraNames();
    Q_INVOKABLE QByteArray defaultCameraName();
    Q_INVOKABLE void initCamera(int cameraIndex);

    Q_INVOKABLE void setState(int cameraIndex, QCamera::State state);
    Q_INVOKABLE void setCaptureMode(int cameraIndex, QCamera::CaptureModes captureMode);
    Q_INVOKABLE void capturePhoto(int cameraIndex, int id, const QString &fileName);
    Q_INVOKABLE QVariant parameter(int cameraIndex, const QString &name);
    Q_INVOKABLE bool setParameter(int cameraIndex, const QString &name, const QVariant &value);
    Q_INVOKABLE QVariantList parameterValues(int cameraIndex, const QString &name, QMetaType::Type valueType) const;

signals:
    void captureModeChanged(int cameraIndex, QCamera::CaptureModes);
    void error(int cameraIndex, int errorCode, const QString &errorString);
    void imageCaptureError(int cameraIndex, int id, int errorCode, const QString &errorString);
    void imageCaptured(int cameraIndex, int id, const QByteArray &imageData, const QString &fileName);
    void previewCaptured(int cameraIndex, const QImage &image);
    void readyForCaptureChanged(int cameraIndex, bool readyForCapture);
    void stateChanged(int cameraIndex, QCamera::State state);
    void statusChanged(int cameraIndex, QCamera::Status status);

private:
    Q_DISABLE_COPY(GPhotoWorker)

    CameraAbilities getCameraAbilities(int cameraIndex, bool *ok = nullptr);
    GPPortInfo getPortInfo(int cameraIndex, bool *ok = nullptr);
    void updateDevices();

    std::map<int, std::unique_ptr<GPhotoCamera>> m_cameras;

    GPContextPtr m_context;
    CameraAbilitiesListPtr m_abilitiesList;
    GPhotoDevices m_devices;
    QMutex m_mutex;
};

#endif // GPHOTOWORKER_H
