#include "mprmpr/server/web_server.h"

#include <string>
#include <gflags/gflags.h>
#include <string.h>
#include <stdlib.h>

#include "mprmpr/base/strings/substitute.h"

using std::string;

namespace mprmpr {

static std::string GetDefaultDocumentRoot();

} // namespace mprmpr

// Flags defining web server behavior. The class implementation should
// not use these directly, but rather access them via WebServerOptions.
// This makes it easier to instantiate web servers with different options
// within a single unit test.
DEFINE_string(webserver_interface, "",
    "Interface to start debug webserver on. If blank, webserver binds to 0.0.0.0");
//TAG_FLAG(webserver_interface, advanced);

DEFINE_string(webserver_doc_root, mprmpr::GetDefaultDocumentRoot(),
    "Files under <webserver_doc_root> are accessible via the debug webserver. "
    "Defaults to $ANT_HOME/www, or if $ANT_HOME is not set, disables the document "
    "root");
//TAG_FLAG(webserver_doc_root, advanced);

DEFINE_bool(webserver_enable_doc_root, true,
    "If true, webserver may serve static files from the webserver_doc_root");
//TAG_FLAG(webserver_enable_doc_root, advanced);

DEFINE_string(webserver_certificate_file, "",
    "The location of the debug webserver's SSL certificate file, in .pem format. If "
    "empty, webserver SSL support is not enabled");
DEFINE_string(webserver_authentication_domain, "",
    "Domain used for debug webserver authentication");
DEFINE_string(webserver_password_file, "",
    "(Optional) Location of .htpasswd file containing user names and hashed passwords for"
    " debug webserver authentication");


DEFINE_int32(webserver_num_worker_threads, 50,
             "Maximum number of threads to start for handling web server requests");
//TAG_FLAG(webserver_num_worker_threads, advanced);

DEFINE_int32(webserver_port, 0,
             "Port to bind to for the web server");
//TAG_FLAG(webserver_port, stable);

namespace mprmpr {

// Returns ANT_HOME if set, otherwise we won't serve any static files.
static string GetDefaultDocumentRoot() {
  char* ant_home = getenv("ANT_HOME");
  // Empty document root means don't serve static files
  return ant_home ? strings::Substitute("$0/www", ant_home) : "";
}

WebServerOptions::WebServerOptions()
  : bind_interface(FLAGS_webserver_interface),
    port(FLAGS_webserver_port),
    doc_root(FLAGS_webserver_doc_root),
    enable_doc_root(FLAGS_webserver_enable_doc_root),
    certificate_file(FLAGS_webserver_certificate_file),
    authentication_domain(FLAGS_webserver_authentication_domain),
    password_file(FLAGS_webserver_password_file),
    num_worker_threads(FLAGS_webserver_num_worker_threads) {
}

} // namespace mprmpr
