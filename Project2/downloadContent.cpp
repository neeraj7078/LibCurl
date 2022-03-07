
#include <iostream>
#include <stdio.h>
#include <curl/curl.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct memory
{
	char *memory;
	size_t size;
};

static size_t writecallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

void getToken()
{
	CURL *curl;
	CURLcode res;
	struct memory chunk;
	chunk.memory = NULL;  /* will be grown as needed by the realloc above */
	chunk.size = 0;       /* no data at this point */

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, "https://ims-na1-stg1.adobelogin.com/ims/token/v1");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Cookie: ftrset=380; relay=c997f1ab-68a0-488a-adc7-52b84a9c6a46");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_mime *mime;
		curl_mimepart *part;
		mime = curl_mime_init(curl);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "password");
		curl_mime_data(part, "New@4112", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "grant_type");
		curl_mime_data(part, "password", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "scope");
		curl_mime_data(part, "AdobeID,openid,creative_cloud", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "client_secret");
		curl_mime_data(part, "d703b2a5-c1d1-474f-9322-cffa1ac85eeb", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "client_id");
		curl_mime_data(part, "adobeID-storage", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "username");
		curl_mime_data(part, "neershar+JP+Complete+VISA+n+1@adobetest.com", CURL_ZERO_TERMINATED);
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writecallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &chunk);

		res = curl_easy_perform(curl);
		curl_mime_free(mime);
	}
	curl_easy_cleanup(curl);

	auto jasonResponse = json::parse(chunk.memory);
	auto imsToken = jasonResponse["access_token"];
	auto imsTokenString = imsToken.get<std::string>();

	std::cout << imsToken.get<std::string>() << '\n';

}

int main(void)
{
	getToken();
	return 0;
}