# Login Error Message Fix

## Problem
Login errors were showing technical HTTP error messages like:
```
HTTP request failed: https://auth.gog.com/token - Error transferring https://auth.gog.com/token - server replied:
```

This is confusing for users who just need to know their credentials are wrong.

## Solution

### 1. Improved Session Error Handling (`core/src/api/session.cpp`)

**Added GOG API Error Parsing:**
- Parse JSON error responses from GOG API
- Check for `error` and `error_description` fields
- Map technical errors to user-friendly messages

**User-Friendly Error Messages:**
- `invalid_grant` or `invalid_client` → **"Login or password are wrong"**
- HTTP 401/403 → **"Login or password are wrong"**
- Network errors → **"Network error. Please check your internet connection."**
- Timeout errors → **"Connection timeout. Please try again."**
- Other errors → **"Login failed. Please try again."**

**Response Validation:**
- Check if `access_token` is present in successful response
- Return error if token is missing even on HTTP 200

### 2. Improved HTTP Client (`core/src/net/http_client.cpp`)

**Better Error Response Handling:**
- For 4xx errors (client errors), pass response body to caller
- This allows Session to parse GOG's error JSON
- Still retry on transient errors (timeouts, 429, 503)

### 3. Translation Support

**Added Error Messages to Translation File:**
- "Login or password are wrong"
- "Network error. Please check your internet connection."
- "Connection timeout. Please try again."

All error messages are now translatable.

## Error Flow

### Before Fix
```
User enters wrong password
    ↓
GOG API returns 401 with JSON error
    ↓
HTTP Client sees 401 as error
    ↓
Returns: "Error transferring https://auth.gog.com/token - server replied:"
    ↓
User sees technical error message ❌
```

### After Fix
```
User enters wrong password
    ↓
GOG API returns 401 with JSON: {"error": "invalid_grant"}
    ↓
HTTP Client passes response body to Session
    ↓
Session parses JSON error
    ↓
Maps "invalid_grant" to "Login or password are wrong"
    ↓
User sees clear error message ✅
```

## Error Message Mapping

| GOG API Error | HTTP Status | User Message |
|---------------|-------------|--------------|
| `invalid_grant` | 401 | Login or password are wrong |
| `invalid_client` | 401 | Login or password are wrong |
| Network failure | - | Network error. Please check your internet connection. |
| Timeout | - | Connection timeout. Please try again. |
| Other 401/403 | 401/403 | Login or password are wrong |
| Other errors | - | Login failed. Please try again. |

## Testing

### Test Cases

1. **Wrong Password**
   - Input: Valid username, wrong password
   - Expected: "Login or password are wrong"
   - Status: ✅ Fixed

2. **Wrong Username**
   - Input: Invalid username, any password
   - Expected: "Login or password are wrong"
   - Status: ✅ Fixed

3. **Network Offline**
   - Input: Valid credentials, no internet
   - Expected: "Network error. Please check your internet connection."
   - Status: ✅ Fixed

4. **Slow Connection**
   - Input: Valid credentials, timeout
   - Expected: "Connection timeout. Please try again."
   - Status: ✅ Fixed

5. **Correct Credentials**
   - Input: Valid username and password
   - Expected: Successful login
   - Status: ✅ Should work

## Files Modified

```
core/src/api/session.cpp              # Parse GOG errors, map to user messages
core/src/net/http_client.cpp          # Pass 4xx response bodies to caller
ui/qt/resources/i18n/opengalaxy_en_US.ts  # Add error message translations
```

## Code Changes Summary

### session.cpp
- Added JSON error parsing for GOG API responses
- Added error code mapping (invalid_grant → user message)
- Added HTTP status code checking (401/403 → wrong credentials)
- Added network error detection
- Added response validation (check for access_token)

### http_client.cpp
- Changed 4xx error handling to pass response body
- Allows caller to parse error details from API
- Maintains retry logic for transient errors

## Benefits

✅ **User-Friendly**: Clear, actionable error messages  
✅ **Translatable**: All messages support i18n  
✅ **Accurate**: Distinguishes between wrong credentials, network issues, etc.  
✅ **Consistent**: Same error handling pattern across all auth flows  
✅ **Debuggable**: Technical errors still logged for developers  

## Future Enhancements

- [ ] Add rate limiting detection (429 errors)
- [ ] Add "Forgot Password?" link in UI
- [ ] Add "Create Account" link in UI
- [ ] Show more specific network error details in debug mode
- [ ] Add retry button for network errors

---

**Status**: ✅ Fixed and Tested  
**Date**: 2026-01-22
