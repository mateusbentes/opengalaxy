#ifndef TRANSLATION_MANAGER_H
#define TRANSLATION_MANAGER_H

#include <QObject>
#include <QTranslator>
#include <QString>

namespace opengalaxy {
namespace ui {

class TranslationManager : public QObject
{
    Q_OBJECT

public:
    explicit TranslationManager(QObject* parent = nullptr);
    ~TranslationManager() override;

    // Get current UI locale (e.g., "en_US", "pt_BR")
    QString currentLocale() const { return currentLocale_; }

    // Get GOG API locale format (e.g., "en-US", "pt-BR")
    QString gogApiLocale() const;

    // Get list of available languages
    QStringList availableLocales() const;

    // Get display name for a locale (e.g., "English", "Português")
    QString localeDisplayName(const QString& locale) const;

    // Set language (will take effect after restart)
    void setLocale(const QString& locale);

    // Load language from settings or system default
    void loadFromSettings();

signals:
    void localeChanged(const QString& newLocale);

private:
    bool loadTranslator(const QString& locale);

    QTranslator translator_;
    QString currentLocale_;
};

} // namespace ui
} // namespace opengalaxy

#endif // TRANSLATION_MANAGER_H
