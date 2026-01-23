# OAuth Login Setup Guide

## Overview

OpenGalaxy uses OAuth 2.0 for secure authentication with GOG. This requires Qt WebEngine to display the GOG login page within the application.

## Why OAuth Instead of Password?

GOG's API **does not support** direct password authentication (`grant_type=password`). The only supported authentication methods are:

1. **OAuth Authorization Code Flow** - User logs in via web browser
2. **Refresh Token** - Renew expired tokens

This is why we need to implement OAuth with an embedded web view.

## Installing Qt WebEngine

### Ubuntu/Debian
```bash
sudo apt install qt6-webengine qt6-webengine-dev libqt6webenginewidgets6
```

### Fedora
```bash
sudo dnf install qt6-qtwebengine qt6-qtwebengine-devel
```

### Arch Linux
```bash
sudo pacman -S qt6-webengine
```

### macOS (Homebrew)
```bash
brew install qt6
# WebEngine is included in qt6
```

### Windows
Qt WebEngine is included in the Qt6 installation from the official installer.

## Rebuilding After Installation

After installing Qt WebEngine:

```bash
cd /home/mateus/opengalaxy
rm -rf build
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```

You should see:
```
-- Qt WebEngine found - OAuth web login enabled
```

## How OAuth Login Works

### 1. User Clicks "Sign In with GOG"
- Application opens an embedded web view
- Loads GOG's login page: `https://auth.gog.com/auth`

### 2. User Logs In
- User enters their GOG credentials in the web view
- GOG authenticates the user
- GOG redirects to: `https://embed.gog.com/on_login_success?code=XXXXX`

### 3. Application Exchanges Code for Tokens
- Application extracts the authorization code from the URL
- Sends GET request to `https://auth.gog.com/token` with:
  - `client_id`
  - `client_secret`
  - `grant_type=authorization_code`
  - `code=XXXXX`
  - `redirect_uri`

### 4. GOG Returns Tokens
- `access_token` - Used for API requests
- `refresh_token` - Used to get new access tokens
- `expires_in` - Token expiration time

### 5. Application Saves Tokens
- Tokens saved to `~/.local/share/opengalaxy/session.json`
- User is logged in and can access their library

## Without Qt WebEngine

If Qt WebEngine is not available, the application will show an error message:

```
OAuth login requires Qt WebEngine which is not available.

Please install qt6-webengine package:
  sudo apt install qt6-webengine

Or use the command-line tool for authentication.
```

### Alternative: Manual OAuth Flow

You can manually authenticate using a web browser:

1. Open this URL in your browser:
```
https://auth.gog.com/auth?client_id=46899977096215655&redirect_uri=https://embed.gog.com/on_login_success?origin=client&response_type=code&layout=client2
```

2. Log in with your GOG account

3. After login, you'll be redirected to a URL like:
```
https://embed.gog.com/on_login_success?origin=client&code=XXXXXXXXXXXXXXXX
```

4. Copy the `code` value from the URL

5. Use the CLI tool to exchange the code for tokens:
```bash
opengalaxy-cli auth --code XXXXXXXXXXXXXXXX
```

## Security Considerations

### Client Credentials

The `client_id` and `client_secret` used are **public credentials** provided by GOG for desktop applications:

```
client_id: 46899977096215655
client_secret: 9d85c43b1482497dbbce61f6e4aa173a433796eeae2ca8c5f6129f2dc4de46d9
```

These are:
- **Not secret** - They're meant to be embedded in applications
- **Same for all GOG clients** - MiniGalaxy, Heroic, etc. use the same credentials
- **Documented** - Available in GOG API documentation

### Token Storage

Tokens are stored in:
```
~/.local/share/opengalaxy/session.json
```

This file:
- Contains your access and refresh tokens
- Should have restricted permissions (600)
- Is automatically created/updated
- Is deleted on logout

### Privacy

- Your password is **never** stored or sent to OpenGalaxy
- Authentication happens directly with GOG's servers
- OpenGalaxy only receives the authorization code and tokens
- Tokens can be revoked at any time from GOG's website

## Troubleshooting

### "Qt WebEngine not found"

**Problem**: CMake can't find Qt WebEngine

**Solution**:
```bash
# Install Qt WebEngine
sudo apt install qt6-webengine qt6-webengine-dev

# Verify installation
dpkg -l | grep qt6-webengine

# Rebuild
cd build && cmake .. && cmake --build .
```

### "OAuth login requires Qt WebEngine"

**Problem**: Application was built without WebEngine support

**Solution**: Install Qt WebEngine and rebuild (see above)

### "Authorization failed"

**Problem**: The authorization code expired or was invalid

**Solution**: Try logging in again. Authorization codes are single-use and expire quickly.

### "Network error"

**Problem**: Can't connect to GOG servers

**Solution**:
- Check your internet connection
- Verify GOG.com is accessible
- Check firewall settings

### Web View Shows Blank Page

**Problem**: WebEngine can't load the page

**Solution**:
- Check internet connection
- Clear WebEngine cache: `rm -rf ~/.local/share/opengalaxy/webengine/`
- Try again

## Testing OAuth

### Test if WebEngine is Available

```bash
# Check if Qt WebEngine is installed
dpkg -l | grep qt6-webengine

# Check if application was built with WebEngine
strings build/ui/opengalaxy | grep HAVE_WEBENGINE
```

### Test OAuth Flow

1. Run the application
2. Click "Sign In with GOG"
3. Web view should open showing GOG login page
4. Enter your credentials
5. After successful login, you should see your library

### Debug OAuth Issues

Enable debug logging:
```bash
export MG_DEBUG=1
./build/ui/opengalaxy
```

Check logs:
```bash
tail -f ~/.local/share/opengalaxy/opengalaxy.log
```

## Comparison with MiniGalaxy

OpenGalaxy's OAuth implementation is based on MiniGalaxy's approach:

| Feature | MiniGalaxy | OpenGalaxy |
|---------|------------|------------|
| **Web View** | GTK WebKit | Qt WebEngine |
| **Client ID** | 46899977096215655 | 46899977096215655 (same) |
| **Redirect URI** | embed.gog.com/on_login_success | embed.gog.com/on_login_success (same) |
| **Grant Type** | authorization_code | authorization_code (same) |
| **Token Exchange** | GET with params | GET with params (same) |

## References

- [GOG API Documentation](https://gogapidocs.readthedocs.io/en/latest/auth.html)
- [MiniGalaxy Source Code](https://github.com/sharkwouter/minigalaxy)
- [OAuth 2.0 Specification](https://oauth.net/2/)
- [Qt WebEngine Documentation](https://doc.qt.io/qt-6/qtwebengine-index.html)

---

**Last Updated**: 2026-01-22  
**Status**: Implemented, requires Qt WebEngine
