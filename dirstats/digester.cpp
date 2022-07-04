#include "digester.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void Digester::invalidate()
{
  if (mdctx_)
  {
    EVP_MD_CTX_free(mdctx_);
    mdctx_ = nullptr;
  }
}

Digester::Digester()
{
  mdctx_ = EVP_MD_CTX_new();
  if (mdctx_)
    if (1 != EVP_DigestInit_ex(mdctx_, EVP_sha256(), NULL))
      invalidate();
}

Digester &Digester::append(const std::string &data)
{
  return append(data.c_str(), data.size());
}

Digester &Digester::append(const char *data, int len)
{
  return append((unsigned char *)data, len);
}

Digester &Digester::append(const unsigned char *data, int len)
{
  if (mdctx_)
    if (1 != EVP_DigestUpdate(mdctx_, data, len))
      invalidate();
  return *this;
}

std::string Digester::finish()
{
  unsigned char digestBuff[EVP_MAX_MD_SIZE];
  unsigned int digestLen = 0;
  static const char *d2hex = "0123456789abcdef";
  if (mdctx_)
    if (1 != EVP_DigestFinal_ex(mdctx_, digestBuff, &digestLen))
      invalidate();
  std::string result;
  if (mdctx_)
    for (unsigned int i = 0; i < digestLen; i++)
    {
      result.push_back(d2hex[digestBuff[i] / 16]);
      result.push_back(d2hex[digestBuff[i] % 16]);
    }
  return result;
}

// returns sha256 digest for a file
// return empty string if file could not be opened
std::string
sha256_from_file(const std::string &fname)
{
  Digester dig;
  int fd = open(fname.c_str(), O_RDONLY);
  if (fd < 0)
    return "";
  char buff[4096];
  while (true)
  {
    ssize_t n = read(fd, buff, sizeof(buff));
    if (n <= 0)
      break;
    dig.append(buff, n);
  }
  close(fd);
  return dig.finish();
}