/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <esp_log.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_netif.h"

#if !CONFIG_IDF_TARGET_LINUX
#include <esp_wifi.h>
#include <esp_system.h>
#include "esp_eth.h"
#endif  // !CONFIG_IDF_TARGET_LINUX
#include "http_server_app.h"

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;

static http_post_callback_t http_post_wheel_callback_t = NULL;
static http_get_callback_t REG = NULL;

// extern const uint8_t index_html_start1[] asm("_binary_wheel_png_start");
// extern const uint8_t index_html_end1[] asm("_binary_wheel_png_end");
extern const uint8_t index_html_start2[] asm("_binary_wheel_html_start");
extern const uint8_t index_html_end2[] asm("_binary_wheel_html_end");

/* An HTTP GET handler */
static esp_err_t server_pdt_handler(httpd_req_t *req)
{
    /* Send response with custom headers and body set as the
     * string passed in user context*/
    // const char* resp_str = (const char*) "Phan Duy Truong";
    // httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    // httpd_resp_set_type(req, "image/png");
    // httpd_resp_send(req, (const char*)index_html_start1, index_html_end1 - index_html_start1);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)index_html_start2, index_html_end2 - index_html_start2);
    return ESP_OK;
}

static const httpd_uri_t server_pdt = {
    .uri       = "/server_pdt",
    .method    = HTTP_GET,
    .handler   = server_pdt_handler,
    .user_ctx  = NULL
};

static esp_err_t wheel_post_handler(httpd_req_t *req)
{
    char buf[100];
    httpd_req_recv(req, buf, req->content_len);
    printf("DATA: %s\n", buf);
    http_post_wheel_callback_t(buf, req->content_len);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t wheel_post_data = {
    .uri       = "/wheel",
    .method    = HTTP_POST,
    .handler   = wheel_post_handler,
    .user_ctx  = NULL
};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/server_pdt", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/server_pdt URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}


void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &server_pdt);
        httpd_register_uri_handler(server, &wheel_post_data);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler); 
        // httpd_register_uri_handler(server, &echo);
        #if CONFIG_EXAMPLE_BASIC_AUTH
        httpd_register_basic_auth(server);
        #endif
    }
    else{
        ESP_LOGI(TAG, "Error starting server!");
    }
}

void stop_webserver(void)
{
    // Stop the httpd server
    httpd_stop(server);
}

void set_post_wheel_callback(void* cb){
    http_post_wheel_callback_t = cb;
}
