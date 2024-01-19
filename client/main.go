package main

import (
	"log"
	"net/http"
)

func main() {
	// Serve static files, e.g., HTML, JS, CSS
	fs := http.FileServer(http.Dir("./static"))
	http.Handle("/", fs)

	// Start HTTP server on port 8081
	log.Println("Listening on http://localhost:8081/")
	err := http.ListenAndServe(":8081", nil)
	if err != nil {
		log.Fatal(err)
	}
}
