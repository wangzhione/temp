package stats

import (
	"testing"
)

func DeepEqual[T comparable](t *testing.T, a, b T) {
	if a != b {
		t.Errorf("a:%+v != b:%+v", a, b)
	}
}

func TestAverage(t *testing.T) {
	t.Parallel()
	DeepEqual(t, Average([]float64{}), 0.0)
	DeepEqual(t, Average([]float64{1}), 1.0)
	DeepEqual(t, Average([]float64{1, 2}), 1.5)
	DeepEqual(t, Average([]float64{1, 2, 3}), 2.0)
}

func TestSum(t *testing.T) {
	t.Parallel()
	DeepEqual(t, Sum([]float64{}), 0.0)
	DeepEqual(t, Sum([]float64{1}), 1.0)
	DeepEqual(t, Sum([]float64{1, 2}), 3.0)
	DeepEqual(t, Sum([]float64{1, 2, 3}), 6.0)
}

func TestPercentiles(t *testing.T) {
	t.Parallel()
	percentiles := map[string]float64{
		"p50": 0.5,
		"p90": 0.9,
	}
	input := []float64{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
	expected := map[string]float64{
		"p50": 5,
		"p90": 9,
	}

	DeepEqual(t, len(percentiles), len(expected))

	for key, value := range Percentiles(input, percentiles) {
		DeepEqual(t, value, expected[key])
	}
}
