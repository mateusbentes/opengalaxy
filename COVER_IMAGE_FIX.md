# Cover Image Loading - Complete Fix

## Problem Summary

Game cover images were failing to load with two different errors:

### Error 1: Protocol Unknown
```
Failed to load cover image: "Protocol \"\" is unknown"
```
**Cause**: GOG API was returning empty strings for some image URLs.

### Error 2: Server Replied Error
```
Failed to load cover image: "Error transferring https://images-4.gog-statics.com/82723c677c894f87bd9be2d1c06dd8781b62417f353ad37e89bd0a35cfc2b3d5 - server replied: "
```
**Cause**: GOG's CDN requires image URLs to have a size suffix (e.g., `_196.jpg`, `_200.jpg`). The hash-based URLs without extensions are rejected by the server.

---

## Root Cause Analysis

GOG's image CDN works as follows:

1. **API Response Format**: 
   - Protocol-relative: `//images-4.gog-statics.com/[hash]`
   - Full HTTPS: `https://images-4.gog-statics.com/[hash]`

2. **CDN Requirements**:
   - URLs must include `https://` protocol
   - URLs must have size suffix: `_196.jpg`, `_200.jpg`, `_300.jpg`, etc.
   - Without suffix, server returns 403/404 error

3. **Example Valid URL**:
   ```
   https://images-4.gog-statics.com/82723c677c894f87bd9be2d1c06dd8781b62417f353ad37e89bd0a35cfc2b3d5_196.jpg
   ```

---

## Solution Implemented

### 1. Enhanced URL Processing in `gog_client.cpp`

**Library Fetch** (lines 70-83):
```cpp
// GOG API returns protocol-relative URLs (//images-X.gog.com/...)
// or full HTTPS URLs. Both need size suffix for CDN.
QString imageUrl = p.value("image").toString();
if (!imageUrl.isEmpty()) {
    // Add https: prefix if protocol-relative
    if (imageUrl.startsWith("//")) {
        imageUrl = "https:" + imageUrl;
    }
    // Add size suffix if not already present
    if (!imageUrl.contains(".jpg") && !imageUrl.contains(".png") && 
        !imageUrl.contains(".webp") && !imageUrl.contains(".gif")) {
        g.coverUrl = imageUrl + "_196.jpg";
    } else {
        g.coverUrl = imageUrl;
    }
}
```

**Store Search** (lines 199-213):
- Same logic applied to store search results
- Ensures consistency across all image loading

### 2. URL Validation in `game_card.cpp`

**Enhanced `loadCoverImage()` method**:
```cpp
void GameCard::loadCoverImage(const QString& url)
{
    // Don't attempt to load if URL is empty or invalid
    if (url.isEmpty() || url.trimmed().isEmpty()) {
        return;
    }

    // Validate URL has a protocol
    QUrl qurl(url);
    if (!qurl.isValid() || qurl.scheme().isEmpty()) {
        qDebug() << "Invalid cover URL (no protocol):" << url;
        return;
    }
    
    // ... rest of network request code
}
```

**Benefits**:
- Prevents network requests for empty URLs
- Validates URL format before making requests
- Reduces console spam from invalid URLs
- Graceful fallback to placeholder emoji (🎮)

---

## Image Size Options

GOG CDN supports multiple image sizes:

| Suffix | Dimensions | Use Case |
|--------|-----------|----------|
| `_196.jpg` | ~196px | Small thumbnails, list views |
| `_200.jpg` | ~200px | Medium thumbnails |
| `_300.jpg` | ~300px | Large cards (current choice) |
| `_392.jpg` | ~392px | Extra large |
| `_ggvgm.jpg` | Variable | Special format |

**Current Implementation**: Uses `_196.jpg` for optimal balance between quality and loading speed.

**To Change Size**: Modify the suffix in both locations in `gog_client.cpp`:
```cpp
g.coverUrl = imageUrl + "_300.jpg";  // For larger images
```

---

## Testing Results

### Before Fix:
- ❌ Empty URLs caused "Protocol unknown" errors
- ❌ Hash URLs without suffix caused server errors
- ❌ No images loaded in library or store

### After Fix:
- ✅ Empty URLs are silently skipped
- ✅ Protocol-relative URLs converted to HTTPS
- ✅ Size suffix automatically appended
- ✅ Images load correctly from GOG CDN
- ✅ Fallback placeholder (🎮) shown when no image available

---

## Files Modified

1. **`core/src/api/gog_client.cpp`**
   - Enhanced `fetchLibrary()` - lines 70-83
   - Enhanced `searchStore()` - lines 199-213
   - Added size suffix logic for GOG CDN

2. **`ui/qt/widgets/game_card.cpp`**
   - Enhanced `loadCoverImage()` - lines 266-310
   - Added URL validation
   - Better error handling
   - Added QUrl and QDebug includes

---

## Future Enhancements

### 1. Adaptive Image Quality
```cpp
// Choose size based on card dimensions
QString suffix = (cardWidth > 300) ? "_392.jpg" : "_196.jpg";
g.coverUrl = imageUrl + suffix;
```

### 2. Image Caching
- Cache downloaded images to disk
- Reduce network requests on app restart
- Implement in `game_card.cpp` using `QPixmapCache` or custom cache

### 3. Lazy Loading
- Load images only when cards are visible
- Implement with `QScrollArea` visibility detection
- Improves initial load time for large libraries

### 4. Retry Logic
```cpp
// Retry with different size if first attempt fails
if (reply->error() == QNetworkReply::ContentNotFoundError) {
    tryAlternativeSize();
}
```

---

## Debugging Tips

### Enable Verbose Logging
Add to `game_card.cpp`:
```cpp
qDebug() << "Loading cover from:" << url;
qDebug() << "Response code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
```

### Test Individual URLs
```bash
# Test if URL works
curl -I "https://images-4.gog-statics.com/[hash]_196.jpg"

# Should return: HTTP/2 200
```

### Check API Response
```bash
# View raw API response
curl "https://embed.gog.com/account/getFilteredProducts?mediaType=1&page=1" \
  -H "Authorization: Bearer [token]" | jq '.products[0].image'
```

---

## Conclusion

✅ **Cover images now load correctly!**

The fix handles:
- Empty/invalid URLs gracefully
- Protocol-relative URLs (//...)
- Full HTTPS URLs
- Missing file extensions
- GOG CDN size suffix requirements

All game cards in the library and store search now display proper cover art from GOG's CDN.
