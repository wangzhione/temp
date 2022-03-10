package main

import (
	"testing"
)

// build:
// go test 12_inline_test.go -bench=.
//

/*
goos: linux
goarch: amd64
cpu: Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
Benchmark_max-8                 1000000000               0.5595 ns/op
Benchmark_maxNoInline-8         1000000000               0.6716 ns/op
PASS
ok      command-line-arguments  1.372s
*/

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

// go: 命令之前不能有空格

//go:noinline
func maxNoInline(a, b int) int {
	if a > b {
		return a
	}
	return b
}

var (
	Result int
)

func Benchmark_max(b *testing.B) {
	var r int
	for i := 0; i < b.N; i++ {
		r = max(r, i)
	}
	Result = r
}

func Benchmark_maxNoInline(b *testing.B) {
	var r int
	for i := 0; i < b.N; i++ {
		r = maxNoInline(r, i)
	}
	Result = r
}

// go test 12_inline_test.go -bench="^Benchmark_max$"
// go test -gcflags="-m -m" 12_inline_test.go
// go build -gcflags="-l" 12_inline_test.go
// go tool compile -l 12_inline_test.go
