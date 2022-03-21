package main

import (
	"bytes"
	"encoding/hex"
	"encoding/json"
	"fmt"
	"testing"
	"time"
)

func FuzzHex(f *testing.F) {
	for _, seed := range [][]byte{{}, {0}, {9}, {0xa}, {0xF}, {1, 2, 3, 4}} {
		f.Add(seed)
	}

	f.Fuzz(func(t *testing.T, in []byte) {
		enc := hex.EncodeToString(in)
		out, err := hex.DecodeString(enc)
		if err != nil {
			t.Fatalf("%+v: devode: %+v", in, err)
		}
		if !bytes.Equal(in, out) {
			t.Fatalf("%+v: not equal: %+v", in, out)
		}
		t.Logf("done")
	})

	now := time.Now()
	fmt.Printf("now = %+v\n", now)

	a := 6
	data, err := json.Marshal(a)
	var dnil []byte
	fmt.Println("json.Marshal", dnil, string(data), err)
}

/*
PS D:\code\github\temp\code\go\example\go1.18> go.exe test -timeout 30s -run ^FuzzHex$ GO1.18 -v
=== RUN   FuzzHex
=== RUN   FuzzHex/seed#0
    main_test.go:23: done
=== RUN   FuzzHex/seed#1
    main_test.go:23: done
=== RUN   FuzzHex/seed#2
    main_test.go:23: done
=== RUN   FuzzHex/seed#3
    main_test.go:23: done
=== RUN   FuzzHex/seed#4
    main_test.go:23: done
=== RUN   FuzzHex/seed#5
    main_test.go:23: done
--- PASS: FuzzHex (0.00s)
    --- PASS: FuzzHex/seed#0 (0.00s)
    --- PASS: FuzzHex/seed#1 (0.00s)
    --- PASS: FuzzHex/seed#2 (0.00s)
    --- PASS: FuzzHex/seed#3 (0.00s)
    --- PASS: FuzzHex/seed#4 (0.00s)
    --- PASS: FuzzHex/seed#5 (0.00s)
PASS
ok      GO1.18  0.210s
*/
