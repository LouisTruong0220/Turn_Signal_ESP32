#ifndef __HTTP_SERVER_APP_H
#define __HTTP_SERVER_APP_H

typedef void (*http_post_callback_t)(char* data, uint16_t len);
typedef void (*http_get_callback_t)(void);

void set_post_wheel_callback(void* cb);
void start_webserver(void);
void stop_webserver(void);
#endif