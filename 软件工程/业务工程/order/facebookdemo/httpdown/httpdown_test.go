package httpdown

import (
	"fmt"
	"testing"
)

func Test_defer(t *testing.T) {
	fn := func() {
		defer func() {
			fmt.Printf("Test_defer defer panic test\n")

			panic("Test_defer panic 2")
		}()

		panic("Test_defer panic 1")
	}

	defer func() {
		rec := recover()
		if rec != nil {
			fmt.Printf("rec = %+v\n", rec)
		}
	}()

	fn()
}
