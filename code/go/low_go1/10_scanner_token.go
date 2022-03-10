package main

import (
	"fmt"
	"go/scanner"
	"go/token"
)

// build:
// go run 10_scanner_token.go
//
func main() {
	code := []byte(`cos(x) + 2i*sin(x) // Euler`)

	// 初始化 scanner
	var scan scanner.Scanner
	tfs := token.NewFileSet()
	file := tfs.AddFile("", tfs.Base(), len(code))
	// scanner.ScanComments return comments as COMMENT tokens
	scan.Init(file, code, nil, scanner.ScanComments)

	// 扫描
	for {
		pos, tok, lit := scan.Scan()
		if tok == token.EOF {
			break
		}
		fmt.Printf("%s\t%s\t%q\n", tfs.Position(pos), tok, lit)
	}
}

// cos(x) + 2i*sin(x) // Euler
/*
1:1     IDENT   "cos"
1:4     (       ""
1:5     IDENT   "x"
1:6     )       ""
1:8     +       ""
1:10    IMAG    "2i"
1:12    IDENT   "sin"
1:15    (       ""
1:16    IDENT   "x"
1:17    )       ""
1:19    ;       "\n"
1:19    COMMENT "// Euler"
*/
