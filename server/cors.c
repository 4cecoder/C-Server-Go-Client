#include "cors.h"


void set_cors_headers(const struct _u_response *response) {
  u_map_put(response->map_header, "Access-Control-Allow-Origin", "*");
  u_map_put(response->map_header, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  u_map_put(response->map_header, "Access-Control-Allow-Headers", "Content-Type, Authorization");
  u_map_put(response->map_header, "Access-Control-Allow-Credentials", "true");
}
