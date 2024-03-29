#pragma once
#include <string>
#include <openssl/crypto.h>
#include <openssl/evp.h>

///
/// Digester is a class to help you compute sha256 digests.  It wraps
/// the relevant OpenSSL C-APIs in a simpale C++ interface.  Here is
/// an example of computing the digest for "Hello world!" string:
///
///   auto digest = Digester().append("Hello").append(" world!").finish();
///
/// .constructor() starts with an empty string, i.e.:
///
///    auto digester = Digester();
///    auto digest = digester.fisnish();
///
/// digest will now contain SHA256 digest of empty data.
///
/// .append() methods append data to the digester
///
/// .finish() returns the final digest as a hexadecimal string (64
/// chars) Do not call .finish() multiple times, the result would be
/// undefined behavior.
///
class Digester {
  EVP_MD_CTX * mdctx_ = nullptr;
  void invalidate();
 public:
  Digester();
  Digester & append( const std::string & data);
  Digester & append( const char * data, int len);
  Digester & append( const unsigned char * data, int len);
  std::string finish();
};

///
/// sha256_from_file() computes sha256 digest of a file
///
/// if digest cannot be computed (eg. file is not readable), then it
/// returns an empty string
///
std::string
sha256_from_file( const std::string & fname);
