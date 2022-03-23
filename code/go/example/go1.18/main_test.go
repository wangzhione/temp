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

func Test_quiz0(t *testing.T) {
	s := []string{"a", "b", "c"}

	copy(s[1:], s)

	println(s)
	fmt.Println(s)
}

/*
 [3/3]0xc000096450
[a a b]

解释:
s = {"a", "b", "c"}

s[1:] <- s

// The copy built-in function copies elements from a source slice into a
// destination slice. (As a special case, it also will copy bytes from a
// string to a slice of bytes.) The source and destination may overlap. Copy
// returns the number of elements copied, which will be the minimum of
// len(src) and len(dst).
func copy(dst, src []Type) int

copy 支持内存重叠拷贝, 意味着底层是 memmove 语法

s[2] = s[3], s[3] = "b" 则 s[2] = "b"
s[1] = s[0], s[0] = "a" 则 s[1] 被修改为 "a"
s[0] = "a"

所以最终结果是 aab
*/

func Test_quiz1(t *testing.T) {
	a := make([]int, 20)

	b := a[18:]
	fmt.Println(len(b), cap(b))

	b = append(b, 2022)

	fmt.Println(len(b), cap(b))

	fmt.Println(b, a)
}

/*
 a := make([]int, 20) len(a) = 20, cap(a) = 20

 b := a[18:], 此刻 len(b) = 2, cap(b) = 2
 b = append(b, 2022), 此刻 len(b) = 3, cap(b) = 4
*/

func Test_quiz2(t *testing.T) {
	c := make(chan int, 1)
	c <- 1
	close(c)
	close(c)
	fmt.Println("OK")
}

/*
multiple close panic

panic: close of closed channel [recovered]
	panic: close of closed channel
*/
