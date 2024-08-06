package main

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
)

// 参照学习素材
// http://httpbin.org/get
// https://cloud.tencent.com/developer/article/1515297
// https://fasionchan.com/golang/practices/http-request/

// build:
// go run 000_http_get.go
func main() {
	client := &http.Client{}
	req, err := http.NewRequest("GET", "http://httpbin.org/get", nil)
	if err != nil {
		log.Fatalf("http.NewRequest error = %+v\n", err)
	}
	req.Header.Add("name", "wangzhi5")
	req.Header.Add("age", "32")

	resp, err := client.Do(req)
	if err != nil {
		log.Fatalf("client.Do error = %+v\n", err)
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("ioutil.ReadAll error = %+v\n", err)
	}

	log.Printf("body = \n%s\n", body)

	var response interface{}
	err = json.Unmarshal(body, &response)
	if err != nil {
		log.Fatalf("json.Unmarshal error = %+v", err)
	}
	log.Printf("response = %+v\n", response)

	var response1 HttpResponse
	err = json.Unmarshal(body, &response1)
	if err != nil {
		log.Fatalf("json.Unmarshal response1 error = %+v", err)
	}
	log.Printf("response1 = %+v\n", response1)
}

/*
{
  "args": {},
  "headers": {
    "Accept-Encoding": "gzip",
    "Age": "32",
    "Host": "httpbin.org",
    "Name": "wangzhi5",
    "User-Agent": "Go-http-client/1.1",
    "X-Amzn-Trace-Id": "Root=1-625e6f40-03ef75182b4d51015cae6bae"
  },
  "origin": "114.247.175.204",
  "url": "http://httpbin.org/get"
}
*/
// HttpResponse 结构
type HttpResponse struct {
	Args map[string]string `json:"args,omitempty"`
	// struct field tag `json:\"headers, omitempty\"` not compatible with reflect.StructTag.Get: suspicious space in struct tag value
	Headers map[string]string `json:"headers,omitempty"`
	Origin  string            `json:"origin,omitempty"`
	Url     string            `json:"url,omitempty"`
}
