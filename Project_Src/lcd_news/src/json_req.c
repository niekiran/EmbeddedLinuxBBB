#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curl/curl.h"

#include "jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

#if 0

static const char *json_string =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

#endif

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}


char URL[] = "https://newsapi.org/v1/articles?source=cnn&sortBy=top&apiKey=d121b6202e754bcf90d278c5b8f5f4ef";

#define BUFFER_SIZE 10000

char buf[BUFFER_SIZE];


/*The maximum amount of body data that will be passed to the write callback is defined in the curl.h header file: CURL_MAX_WRITE_SIZE (the usual default is 16K).*/

/*The data passed to this function will not be zero terminated!*/

static size_t fetch_data(void *data_ptr_rcvd, size_t size, size_t nmemb, void *userp)
{
  //  buf_t *buf = (buf_t *) userp;
    size_t total_size = size * nmemb;
    
    if(total_size > BUFFER_SIZE )
     total_size = BUFFER_SIZE;

   memcpy(userp,data_ptr_rcvd,BUFFER_SIZE);

   char *buffer = (char*)userp;
   buffer[BUFFER_SIZE-1] = '\0';
 


    return total_size;
}



char * json_fetch(char *url)
{
    CURL *curl = curl_easy_init();
    if(! curl) 
    {
	printf("curl init faild . exiting \n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);


    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fetch_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)buf);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "jsmn-example (https://github.com/alisdair/jsmn-example, alisdair@mcdiarmid.org)");

    struct curl_slist *hs = curl_slist_append(NULL, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        printf("curl_easy_perform failed: %s\n", curl_easy_strerror(res));

    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);

    buf[BUFFER_SIZE-1] = '\0';


    return buf;
}


int main() {
	int i;
	int r;
        const char *json_string = json_fetch(URL);
        if(! json_string)
        {
        	printf("JSON fetch from URL failed .. exiting \n");
        	return -1;
        }
	jsmn_parser p;
	jsmntok_t t[128 * 2]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, json_string, strlen(json_string), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d %s\n", r, json_string);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(json_string, &t[i], "source") == 0) {
			/* We may use strndup() to fetch string value */
			printf("-Source-: %.*s\n", t[i+1].end-t[i+1].start,
					json_string + t[i+1].start);
			i++;
		} else if (jsoneq(json_string, &t[i], "author") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("-Author-: %.*s\n", t[i+1].end-t[i+1].start,
					json_string + t[i+1].start);
			i++;
		} else if (jsoneq(json_string, &t[i], "title") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- Title-: %.*s\n", t[i+1].end-t[i+1].start,
					json_string + t[i+1].start);
			i++;
		} else {
                           ;
		}
	}
	return EXIT_SUCCESS;
}
