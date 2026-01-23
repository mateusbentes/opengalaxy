# OpenGalaxy Multilingual System Guide

## Overview

OpenGalaxy now supports multiple languages using Qt's standard translation system. The system automatically detects your system language and falls back to English if the language is not available.

## Current Features

### ✅ Implemented
- **Auto-detection**: Automatically detects system language on first run
- **English fallback**: Falls back to English if selected language is unavailable
- **Language selection**: Users can change language in Settings page
- **GOG API locale**: Store searches and game info use the selected language
- **Restart notification**: Users are notified that restart is required for language changes
- **Persistent settings**: Language preference is saved and restored

### 🌍 Supported Languages
- **English (en_US)** - Default, always available

### 🔜 Ready to Add
The system is ready to support additional languages. Translation files can be added for:
- Portuguese (pt_BR)
- Spanish (es_ES)
- French (fr_FR)
- German (de_DE)
- Russian (ru_RU)
- Chinese Simplified (zh_CN)
- Japanese (ja_JP)

## File Structure

```
ui/qt/
├── i18n/
│   ├── translation_manager.h      # Translation manager class
│   └── translation_manager.cpp    # Implementation
└── resources/
    └── i18n/
        ├── opengalaxy_en_US.ts    # English translation source (XML)
        └── opengalaxy_en_US.qm    # Compiled translation (binary)
```

## How It Works

### 1. Translation Files (.ts)
- **Format**: XML files containing source strings and translations
- **Location**: `ui/qt/resources/i18n/opengalaxy_<locale>.ts`
- **Example**: `opengalaxy_en_US.ts`, `opengalaxy_pt_BR.ts`
- **Human-readable**: Can be edited with any text editor or Qt Linguist

### 2. Compiled Translations (.qm)
- **Format**: Binary files loaded at runtime
- **Generated from**: .ts files using `lrelease` command
- **Embedded**: Included in application via resources.qrc

### 3. TranslationManager
- Loads translations at startup
- Detects system language
- Manages language switching
- Converts UI locale (en_US) to GOG API locale (en-US)

### 4. Translatable Strings
All user-visible strings use `tr()` function:
```cpp
QLabel* label = new QLabel(tr("Welcome to OpenGalaxy"));
```

## Adding a New Language

### Step 1: Create Translation File

Create a new .ts file for your language:
```bash
cd /home/mateus/opengalaxy
cp ui/qt/resources/i18n/opengalaxy_en_US.ts ui/qt/resources/i18n/opengalaxy_pt_BR.ts
```

### Step 2: Translate Strings

Edit the new .ts file and translate the strings:
```xml
<message>
    <source>Welcome to OpenGalaxy</source>
    <translation>Bem-vindo ao OpenGalaxy</translation>
</message>
```

**Tip**: Use Qt Linguist GUI tool for easier translation:
```bash
linguist ui/qt/resources/i18n/opengalaxy_pt_BR.ts
```

### Step 3: Compile Translation

Compile the .ts file to .qm:
```bash
lrelease ui/qt/resources/i18n/opengalaxy_pt_BR.ts -qm ui/qt/resources/i18n/opengalaxy_pt_BR.qm
```

### Step 4: Add to Resources

Edit `ui/qt/resources/resources.qrc` and add the new .qm file:
```xml
<qresource prefix="/i18n">
    <file>i18n/opengalaxy_en_US.qm</file>
    <file>i18n/opengalaxy_pt_BR.qm</file>
</qresource>
```

### Step 5: Update CMakeLists.txt

Add the new .ts file to `ui/CMakeLists.txt`:
```cmake
set(TS_FILES
    qt/resources/i18n/opengalaxy_en_US.ts
    qt/resources/i18n/opengalaxy_pt_BR.ts
)
```

### Step 6: Rebuild

```bash
cd build
cmake --build . -j$(nproc)
```

## Updating Translations

When you add new translatable strings to the code:

### 1. Extract New Strings

Use `lupdate` to extract new strings from source code:
```bash
lupdate ui/qt/*.cpp ui/qt/**/*.cpp -ts ui/qt/resources/i18n/opengalaxy_en_US.ts
```

### 2. Translate New Strings

Edit the .ts file and add translations for new `<message>` entries.

### 3. Recompile

```bash
lrelease ui/qt/resources/i18n/opengalaxy_en_US.ts -qm ui/qt/resources/i18n/opengalaxy_en_US.qm
```

### 4. Rebuild Application

```bash
cd build
cmake --build .
```

## Translated Components

### Main Window
- Window title
- Sidebar menu items (Library, Store, Friends, Settings)
- Error messages

### Login Page
- Welcome message
- Subtitle
- Input placeholders (Email, Password)
- Sign In button
- Error messages

### Library Page
- Page title
- Search placeholder
- Buttons (Refresh, Play, Install, Details)
- Notifications (Installing, Completed, Failed)
- Error messages

### Store Page
- Page title
- Search placeholder
- Error messages
- **Game titles and descriptions** (from GOG API in selected language)

### Settings Page
- Section titles
- Language selector
- All settings labels
- Restart notification

### Dialogs
- All message boxes
- Button labels
- Error messages

## GOG API Locale Integration

The system automatically sets the GOG API locale based on the selected UI language:

- **UI Language**: `en_US` → **GOG API**: `en-US`
- **UI Language**: `pt_BR` → **GOG API**: `pt-BR`

This means:
- Store search results show game titles in the selected language
- Game descriptions are in the selected language
- Download information respects the locale

## Testing

### Test Language Detection
1. Run the application
2. Check that it detects your system language
3. If not available, verify it falls back to English

### Test Language Switching
1. Open Settings page
2. Change language in the dropdown
3. Verify restart notification appears
4. Restart application
5. Verify UI is in the new language

### Test GOG API Locale
1. Change language to Portuguese (when available)
2. Search for a game in the Store
3. Verify game titles appear in Portuguese

## Troubleshooting

### Language not appearing in dropdown
- Verify .qm file exists in `ui/qt/resources/i18n/`
- Check that .qm file is listed in `resources.qrc`
- Rebuild the application

### Translations not working
- Verify .ts file is compiled to .qm
- Check that `tr()` is used for all strings (not hardcoded)
- Ensure TranslationManager is initialized in main.cpp

### GOG API still in English
- Verify `gogClient_.setLocale()` is called in StorePage constructor
- Check that locale format is correct (en-US, not en_US)
- Some GOG content may not be available in all languages

## Best Practices

### For Developers

1. **Always use tr()** for user-visible strings:
   ```cpp
   // Good
   label->setText(tr("Hello"));
   
   // Bad
   label->setText("Hello");
   ```

2. **Use context** for disambiguation:
   ```cpp
   // In different classes, same string can have different translations
   tr("Open");  // File menu
   tr("Open");  // Door status
   ```

3. **Avoid string concatenation**:
   ```cpp
   // Bad
   tr("You have ") + count + tr(" messages");
   
   // Good
   tr("You have %1 messages").arg(count);
   ```

4. **Extract strings regularly**:
   ```bash
   lupdate ui/qt/**/*.cpp -ts ui/qt/resources/i18n/*.ts
   ```

### For Translators

1. **Use Qt Linguist** for easier translation
2. **Keep formatting** placeholders like `%1`, `%2`
3. **Test translations** in the actual UI
4. **Consider context** - same English word may need different translations
5. **Keep length similar** to avoid UI layout issues

## Future Enhancements

- [ ] Add more languages (Portuguese, Spanish, French, etc.)
- [ ] Implement dynamic language switching (without restart)
- [ ] Add language-specific date/time formatting
- [ ] Support RTL languages (Arabic, Hebrew)
- [ ] Add plural forms support
- [ ] Create translation contribution guide

## Contributing Translations

We welcome translations! To contribute:

1. Fork the repository
2. Create a new translation file for your language
3. Translate all strings
4. Test the translation
5. Submit a pull request

For questions or help, please open an issue on GitHub.

---

**Last Updated**: 2026-01-22
**Version**: 1.0.0
