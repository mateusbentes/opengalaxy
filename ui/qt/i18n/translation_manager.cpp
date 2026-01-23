#include "translation_manager.h"

#include <QApplication>
#include <QSettings>
#include <QLocale>
#include <QDir>
#include <QDebug>
#include "opengalaxy/util/config.h"

namespace opengalaxy {
namespace ui {

TranslationManager::TranslationManager(QObject* parent)
    : QObject(parent)
    , currentLocale_("en_US")
{
}

TranslationManager::~TranslationManager() = default;

QString TranslationManager::gogApiLocale() const
{
    // Convert Qt locale format (en_US) to GOG API format (en-US)
    QString apiLocale = currentLocale_;
    apiLocale.replace('_', '-');
    return apiLocale;
}

QStringList TranslationManager::availableLocales() const
{
    QStringList locales;
    locales << "en_US"; // English is always available (source language)

    // Check for available translation files
    QDir resourceDir(":/i18n");
    if (resourceDir.exists()) {
        QStringList qmFiles = resourceDir.entryList(QStringList() << "opengalaxy_*.qm", QDir::Files);
        for (const QString& file : qmFiles) {
            // Extract locale from filename: opengalaxy_pt_BR.qm -> pt_BR
            QString locale = file;
            locale.remove("opengalaxy_").remove(".qm");
            if (!locales.contains(locale)) {
                locales << locale;
            }
        }
    }

    return locales;
}

QString TranslationManager::localeDisplayName(const QString& locale) const
{
    // Map locale codes to display names
    static QMap<QString, QString> displayNames = {
        {"en_US", "English"},
        {"pt_BR", "Português (Brasil)"},
        {"es_ES", "Español"},
        {"fr_FR", "Français"},
        {"de_DE", "Deutsch"},
        {"ru_RU", "Русский"},
        {"zh_CN", "简体中文"},
        {"ja_JP", "日本語"}
    };

    if (displayNames.contains(locale)) {
        return displayNames[locale];
    }

    // Fallback: use Qt's locale name
    QLocale qlocale(locale);
    return qlocale.nativeLanguageName();
}

bool TranslationManager::loadTranslator(const QString& locale)
{
    // Remove previous translator
    qApp->removeTranslator(&translator_);

    // English is the source language, no translation file needed
    if (locale == "en_US") {
        return true;
    }

    // Try to load translation from resources
    QString qmPath = QString(":/i18n/opengalaxy_%1.qm").arg(locale);

    if (translator_.load(qmPath)) {
        qApp->installTranslator(&translator_);
        qDebug() << "Loaded translation:" << qmPath;
        return true;
    }

    qWarning() << "Failed to load translation:" << qmPath;
    return false;
}

void TranslationManager::setLocale(const QString& locale)
{
    if (currentLocale_ == locale) {
        return;
    }

    currentLocale_ = locale;

    // Save to settings using Config
    opengalaxy::util::Config::instance().setLanguage(locale);

    // Load translator (will take effect after restart)
    loadTranslator(locale);

    emit localeChanged(locale);

    qDebug() << "Language changed to:" << locale << "(restart required)";
}

void TranslationManager::loadFromSettings()
{
    QString savedLocale = opengalaxy::util::Config::instance().language();

    if (!savedLocale.isEmpty()) {
        // Use saved locale
        currentLocale_ = savedLocale;
        qDebug() << "Using saved locale:" << currentLocale_;
    } else {
        // Auto-detect system locale
        QString systemLocale = QLocale::system().name(); // e.g., "en_US", "pt_BR"
        qDebug() << "System locale detected:" << systemLocale;

        // Check if we have a translation for this locale
        QStringList available = availableLocales();
        if (available.contains(systemLocale)) {
            currentLocale_ = systemLocale;
            qDebug() << "Using system locale:" << currentLocale_;
        } else {
            // Fallback to English
            currentLocale_ = "en_US";
            qDebug() << "Falling back to English";
        }

        // Save the detected/fallback locale
        opengalaxy::util::Config::instance().setLanguage(currentLocale_);
    }

    // Load the translator
    loadTranslator(currentLocale_);
}

} // namespace ui
} // namespace opengalaxy
