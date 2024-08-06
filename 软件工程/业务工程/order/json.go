package main

import (
	"encoding/json"
	"log"
)

type Extend struct {
	Name string `json:"name,omitempty"`
}

type Omitempty struct {
	Age  int    `json:"xx,omitempty"`
	Name string `json:"xx,omitempty"`
}

func main() {
	var a Omitempty = Omitempty{
		Age:  1,
		Name: "json",
	}

	data, err := json.Marshal(a)
	if err != nil {
		log.Printf("json.Marshal a = %+v\n", a)
	}
	log.Printf("data = %s\n", data)
}
