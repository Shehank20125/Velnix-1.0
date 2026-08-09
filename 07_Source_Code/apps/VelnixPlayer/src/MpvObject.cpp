#include "MpvObject.h"

#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QMetaObject>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QGuiApplication>
#include <QOpenGLContext>

namespace {

// mpv wants raw void* function pointers to resolve GL functions — this
// bridges that to Qt's OpenGL context.
static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx)
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

// Converts a QVariant into an mpv_node-friendly string list for simple
// command dispatch (covers the common "loadfile", "seek", etc. cases).
static QStringList variantToStringList(const QVariant &v)
{
    QStringList out;
    if (v.canConvert<QVariantList>()) {
        for (const QVariant &item : v.toList())
            out << item.toString();
    } else {
        out << v.toString();
    }
    return out;
}

} // namespace

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    explicit MpvRenderer(MpvObject *obj) : m_obj(obj) {}

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
    {
        return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
    }

    void render() override
    {
        if (!m_obj->m_mpvGL)
            return;

        QOpenGLFramebufferObject *fbo = framebufferObject();
        mpv_opengl_fbo mpfbo{};
        mpfbo.fbo = static_cast<int>(fbo->handle());
        mpfbo.w = fbo->width();
        mpfbo.h = fbo->height();
        mpfbo.internal_format = 0;

        int flip_y = 1;

        mpv_render_param params[] = {
            {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
            {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
            {MPV_RENDER_PARAM_INVALID, nullptr}
        };

        mpv_render_context_render(m_obj->m_mpvGL, params);
    }

private:
    MpvObject *m_obj;
};

MpvObject::MpvObject(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
{
    m_mpv = mpv_create();
    if (!m_mpv)
        qFatal("Failed to create mpv instance");

    // Hardware decoding — this is what makes 4K playback smooth. "auto"
    // picks the best backend available (D3D11VA on Windows, VideoToolbox
    // on macOS, VAAPI on Linux).
    mpv_set_option_string(m_mpv, "hwdec", "auto");
    mpv_set_option_string(m_mpv, "vo", "libmpv");
    mpv_set_option_string(m_mpv, "keep-open", "yes");

    if (mpv_initialize(m_mpv) < 0)
        qFatal("mpv_initialize failed");

    // Observe the properties the UI binds to — mpv pushes updates via
    // events rather than polling.
    mpv_observe_property(m_mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "media-title", MPV_FORMAT_STRING);

    mpv_set_wakeup_callback(m_mpv, [](void *ctx) {
        QMetaObject::invokeMethod(static_cast<MpvObject *>(ctx), "onMpvEvents", Qt::QueuedConnection);
    }, this);

    mpv_opengl_init_params gl_init_params{};
    gl_init_params.get_proc_address = get_proc_address;

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&m_mpvGL, m_mpv, params) < 0)
        qFatal("Failed to create mpv render context");

    mpv_render_context_set_update_callback(m_mpvGL, [](void *ctx) {
        QMetaObject::invokeMethod(static_cast<MpvObject *>(ctx), "update", Qt::QueuedConnection);
    }, this);
}

MpvObject::~MpvObject()
{
    if (m_mpvGL)
        mpv_render_context_free(m_mpvGL);
    if (m_mpv)
        mpv_terminate_destroy(m_mpv);
}

QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const
{
    return new MpvRenderer(const_cast<MpvObject *>(this));
}

void MpvObject::loadFile(const QString &path)
{
    QByteArray pathUtf8 = path.toUtf8();
    const char *args[] = {"loadfile", pathUtf8.constData(), nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvObject::command(const QVariant &params)
{
    QStringList args = variantToStringList(params);
    QVector<QByteArray> utf8Args;
    QVector<const char *> cArgs;
    for (const QString &a : args) {
        utf8Args.append(a.toUtf8());
        cArgs.append(utf8Args.last().constData());
    }
    cArgs.append(nullptr);
    mpv_command_async(m_mpv, 0, cArgs.data());
}

void MpvObject::setMpvProperty(const QString &name, const QVariant &value)
{
    // Handles the common cases (double, bool, string) used by filter
    // sliders like brightness/contrast/saturation/gamma/hue and toggles.
    QByteArray nameUtf8 = name.toUtf8();
    if (value.typeId() == QMetaType::Double || value.typeId() == QMetaType::Int) {
        double d = value.toDouble();
        mpv_set_property(m_mpv, nameUtf8.constData(), MPV_FORMAT_DOUBLE, &d);
    } else if (value.typeId() == QMetaType::Bool) {
        int flag = value.toBool() ? 1 : 0;
        mpv_set_property(m_mpv, nameUtf8.constData(), MPV_FORMAT_FLAG, &flag);
    } else {
        QByteArray strUtf8 = value.toString().toUtf8();
        const char *str = strUtf8.constData();
        mpv_set_property(m_mpv, nameUtf8.constData(), MPV_FORMAT_STRING, &str);
    }
}

QVariant MpvObject::getMpvProperty(const QString &name) const
{
    QByteArray nameUtf8 = name.toUtf8();
    char *result = mpv_get_property_string(m_mpv, nameUtf8.constData());
    if (!result)
        return QVariant();
    QString value = QString::fromUtf8(result);
    mpv_free(result);
    return value;
}

void MpvObject::setPosition(double value)
{
    if (qFuzzyCompare(m_position, value))
        return;
    mpv_set_property(m_mpv, "time-pos", MPV_FORMAT_DOUBLE, &value);
    // m_position updates via the property-change event once mpv confirms the seek.
}

void MpvObject::setPause(bool value)
{
    if (m_pause == value)
        return;
    int flag = value ? 1 : 0;
    mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &flag);
    // m_pause updates via the property-change event, same as position.
}

// --- velnix::core::IPlaybackBackend -----------------------------------
// Thin adapters onto the existing QML-facing methods/properties above —
// same mpv_handle, just reached through Core's Path/Milliseconds types
// instead of QString/double.

void MpvObject::Open(const velnix::core::Path &file)
{
    loadFile(QString::fromStdString(file.string()));
}

void MpvObject::Close()
{
    const char *args[] = {"stop", nullptr};
    mpv_command_async(m_mpv, 0, args);
}

void MpvObject::Play()
{
    setPause(false);
}

void MpvObject::Pause()
{
    setPause(true);
}

void MpvObject::Stop()
{
    Close();
}

void MpvObject::Seek(velnix::core::Milliseconds position)
{
    setPosition(static_cast<double>(position.count()) / 1000.0);
}

velnix::core::Milliseconds MpvObject::GetPosition() const
{
    return velnix::core::Milliseconds(static_cast<qint64>(m_position * 1000.0));
}

velnix::core::Milliseconds MpvObject::GetDuration() const
{
    return velnix::core::Milliseconds(static_cast<qint64>(m_duration * 1000.0));
}

bool MpvObject::IsPaused() const
{
    return m_pause;
}

void MpvObject::onMpvEvents()
{
    while (m_mpv) {
        mpv_event *event = mpv_wait_event(m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
            break;
        handleEvent(event);
    }
}

void MpvObject::handleEvent(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE: {
        auto *prop = static_cast<mpv_event_property *>(event->data);
        const QString name = QString::fromUtf8(prop->name);

        if (name == "time-pos" && prop->format == MPV_FORMAT_DOUBLE) {
            m_position = *static_cast<double *>(prop->data);
            emit positionChanged();
        } else if (name == "duration" && prop->format == MPV_FORMAT_DOUBLE) {
            m_duration = *static_cast<double *>(prop->data);
            emit durationChanged();
        } else if (name == "pause" && prop->format == MPV_FORMAT_FLAG) {
            m_pause = *static_cast<int *>(prop->data) != 0;
            emit pauseChanged();
        } else if (name == "media-title" && prop->format == MPV_FORMAT_STRING) {
            m_mediaTitle = QString::fromUtf8(*static_cast<char **>(prop->data));
            emit mediaTitleChanged();
        }
        break;
    }
    default:
        break;
    }
}
